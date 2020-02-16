#include "gtest/gtest.h"
#include <memory>
#include <chrono>

#include "hermes.h"

namespace hermes {

class TestCRDensity: public CosmicRayDensity {
private:
	QEnergy E_c;
	QFrequency freq;
public:
        TestCRDensity(QFrequency freq_) {
		freq = freq_;
		E_c = sqrt(m_electron * freq * 4_pi / (3*e_plus*1_muG)) 
			* m_electron * c_squared;

		// add some energies over which the integrator integrates
		// including E(freq_c) for which dn/dE contributes only
		// deltaE = 1 eV
		energyRange.push_back(E_c-2_eV);
		energyRange.push_back(E_c-1_eV);
		energyRange.push_back(E_c);
		energyRange.push_back(E_c+1_eV);
		energyRange.push_back(E_c+2_eV);
	}
	QEnergy getElectronEnergy() {
		return E_c;
	}
	// dn/dE is delta-function reproduce F(1) \approx 0.655
        QPDensityPerEnergy getDensityPerEnergy(const QEnergy& E_, const Vector3QLength& pos_) const {
		if (E_ == E_c)
			return 1.0 / (1_m3 * 1_J);
		else
			return 0;
	}
};

TEST(SynchroIntegrator, totalEnergyLoss) {
	/*
 	 * -dE/dt = int_0^inf j(w) dw
 	 *  	  = (e^4/(6*pi*epsilon0^2*c^4*m_e^2))
 	 *  	  	*c*B^2/(2mu0)*gamma^2*sin^2(alpha)*2
 	 */

	auto B = Vector3QMField(0, 0, 1_muG);
        auto mfield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto crdensity = std::make_shared<TestCRDensity>(TestCRDensity(1_MHz));
	auto electronEnergy = crdensity->getElectronEnergy();
	
	auto integrator = std::make_shared<SynchroIntegrator>(
		SynchroIntegrator(mfield, crdensity));

	QPower totalEnergyLoss(0), expectedEnergyLoss(0);

	QFrequency delta_w(1e4);
	for (auto w = 0_Hz; w < 100_MHz; w += delta_w) {
		totalEnergyLoss += integrator->singleElectronEmission(
					w/2_pi, electronEnergy, B.getR()) * delta_w;
	}

	QArea sigma_Thomson = pow<4>(e_plus) / 
				(6_pi*pow<2>(epsilon0*m_electron)*pow<4>(c_light));
	QEnergyDensity U_mag = pow<2>(B.getR())/(2*mu0);
	expectedEnergyLoss = 2*sigma_Thomson*c_light*U_mag*std::pow(sin(90_deg),2) *
				pow<2>(getLorentzFactor(m_electron, electronEnergy));

	EXPECT_NEAR(static_cast<double>(expectedEnergyLoss), static_cast<double>(totalEnergyLoss), 1e-32);
}


TEST(SynchroIntegrator, integrateOverEnergy) {
	/*
	 * Artificial example:
	 * dn/dE = 1 m^(-3)*J^(-1) for freq/freq_c = 1 else 0
	 * deltaE = 1 eV
	 * B_perp = 1 muG
	 * F(1) = 0.655 (from Table 8.1 in Longair)
	 * J = sqrt(3)*electron^3/(8*pi^2*epsilon_0*c*electron_mass)*dn/dE*F(1)*B_perp*deltaE
	*/
	
	auto B = Vector3QMField(0, 0, 1_muG);
	auto crdensity = std::make_shared<TestCRDensity>(TestCRDensity(1_MHz));
        auto mfield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));

	auto integrator = std::make_shared<SynchroIntegrator>(SynchroIntegrator(mfield, crdensity));

	QEmissivity emissivity = integrator->integrateOverEnergy(
			Vector3QLength(1_pc,0,0),
			1_MHz);

	// sqrt(3)*e_charge^3/(8*pi^2*epsilon_0*c*electron_mass)*0.655*1*microGauss*1/(m^3*J)*1_eV
	EXPECT_NEAR(static_cast<double>(emissivity), 3.915573e-55, 2e-56); // J/m^3
}

TEST(SynchroIntegrator, integrateOverLOS) {
	QMField Bamp = 1_muG;
	QFrequency freq = 1_MHz;
	QEnergy E_c = sqrt(m_electron * freq * 4_pi / (3*e_plus*Bamp)) 
			* m_electron * c_squared;
			
	auto B = Vector3QMField(0, 0, 1_muG);
        auto mfield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto crdensity = std::make_shared<TestCRDensity>(TestCRDensity(freq));
	
	auto intSynchro = std::make_shared<SynchroIntegrator>(
		SynchroIntegrator(mfield, crdensity));

	QDirection direction;
        direction[0] = 90_deg;
        direction[1] = 180_deg;
	// dist = 11.5_kpc
	QTemperature T = intSynchro->integrateOverLOS(direction, 1_MHz);
	// look above
	// 3.915573e-55 * 11.5_kpc
	QTemperature T_expected = intensityToTemperature(
		QIntensity(1.3892e-34)/4_pi, freq);
	EXPECT_NEAR(static_cast<double>(T), static_cast<double>(T_expected), 1e-9); // K
}

TEST(SynchroIntegrator, PerformanceTest) {
        auto mfield = std::make_shared<JF12Field>(JF12Field());
	std::vector<PID> particletypes = {Electron, Positron};
	auto dragonModel = std::make_shared<Dragon2DCRDensity>(Dragon2DCRDensity(
				getDataPath("CosmicRays/Gaggero17/run_2D.fits.gz"),
				particletypes)); 
	auto in = std::make_shared<SynchroIntegrator>(SynchroIntegrator(mfield, dragonModel));
	auto skymap = std::make_shared<RadioSkymap>(RadioSkymap(4, 1_GHz));
	skymap->setIntegrator(in);

        std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	skymap->compute();
        std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	float pxl_speed = milliseconds.count()/skymap->getNpix()*getThreadsNumber();

        EXPECT_LE(pxl_speed, 40); // ms
}


int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


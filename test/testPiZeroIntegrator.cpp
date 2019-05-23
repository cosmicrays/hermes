#include "gtest/gtest.h"
#include <memory>

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

TEST(PiZeroIntegrator, integrateOverEnergy) {
	//auto crdensity = std::make_shared<TestCRDensity>(TestCRDensity(1_MHz));
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	
	std::vector<PID> particletypes = {Proton};
	auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity(
				getDataPath("DragonRuns/run_2D.fits.gz"),
				particletypes, DragonFileType::_2D)); 
	// interaction
	auto kamae = std::make_shared<Kamae06>(Kamae06());
	// HI model
	auto ringModel = std::make_shared<RingModelDensity>(RingModelDensity());
	// integrator
	auto intPiZero = std::make_shared<PiZeroIntegrator>(
		PiZeroIntegrator(dragonModel, ringModel, kamae));

	// skymap
	int nside = 4;
	auto skymap = std::make_shared<DiffFluxSkymap>(DiffFluxSkymap(nside, 1_GeV));
	skymap->setIntegrator(intPiZero);

	auto output = std::make_shared<FITSOutput>(FITSOutput("!test-pion.fits.gz"));
	
	auto pos = Vector3QLength(8.5_kpc, 0, 0);
	QDirection dir = {90_deg,1_deg};
	//std::cerr << intPiZero->integrateOverEnergy(pos, 1_GeV) << std::endl;
	//std::cerr << intPiZero->integrateOverEnergy(pos, 100_MeV) << std::endl;
	std::cerr << intPiZero->integrateOverLOS(dir, 1000_MeV) << std::endl;
	//skymap->compute();
	//skymap->save(output);

	// sqrt(3)*e_charge^3/(8*pi^2*epsilon_0*c*electron_mass)*0.655*1*microGauss*1/(m^3*J)*1_eV
	//EXPECT_NEAR(emissivity.getValue(), 3.915573e-55, 2e-56); // J/m^3
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


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
	// dn/dE is delta-function reproduce F(1) \approx 0.655
        QPDensityPerEnergy getDensityPerEnergy(const QEnergy& E_, const Vector3QLength& pos_) const {
		if (E_ == E_c)
			return 1.0 / (1_m3 * 1_J);
		else
			return 0;
	}
};

TEST(Integrator, integrateOverEnergy) {
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

	EXPECT_NEAR(emissivity.getValue(), 3.91e-55, 2e-56); // J/m^3
}

TEST(Integrator, intensityToTemperature) {
	auto B = Vector3QMField(0, 0, 1_muG);
	auto crdensity = std::make_shared<TestCRDensity>(TestCRDensity(1_MHz));
        auto mfield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto integrator = std::make_shared<SynchroIntegrator>(SynchroIntegrator(mfield, crdensity));

	// T = I * c^2 / (2 * nu^2 * k_boltzmann)
	QIntensity intensity(1);
	QTemperature temp = integrator->intensityToTemperature(intensity, 1_Hz);

	EXPECT_NEAR(temp.getValue(), 3.2548e39, 1e36);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


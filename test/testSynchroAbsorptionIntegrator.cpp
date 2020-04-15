#include "gtest/gtest.h"
#include <memory>

#include "hermes.h"

namespace hermes {

class TestChargedGasDensity: public chargedgas::ChargedGasDensity {
public:
        TestChargedGasDensity() : chargedgas::ChargedGasDensity(1e4_K) { }
        QPDensity getDensity(const Vector3QLength &pos) const {
		return 1.0 / 1_cm3; 
	}
};

class TestCRDensity: public cosmicrays::CosmicRayDensity {
private:
	QEnergy E_c;
public:
        TestCRDensity() {
		QFrequency freq = 22_MHz;
		E_c = sqrt(m_electron * freq * 4_pi / (3*e_plus*1_muG)) 
			* m_electron * c_squared;

		// add some energies over which the integrator integrates
		// including E(freq_c) for which dn/dE contributes only
		// deltaE = 1 eV
		energyRange.push_back(E_c-2_GeV);
		energyRange.push_back(E_c-1_GeV);
		energyRange.push_back(E_c);
		energyRange.push_back(E_c+1_GeV);
		energyRange.push_back(E_c+2_GeV);
	}
        QPDensityPerEnergy getDensityPerEnergy(const QEnergy& E_, const Vector3QLength& pos_) const {
		return 1.0 / (1_m3 * 1_J);
	}
};

TEST(SynchroAbsorptionIntegrator, integrateOverLOS) {
	auto B = Vector3QMField(0, 0, 1_muG);
        auto mfield = std::make_shared<magneticfields::UniformMagneticField>(magneticfields::UniformMagneticField(B));
	auto gdensity = std::make_shared<TestChargedGasDensity>(TestChargedGasDensity());
	auto crdensity = std::make_shared<TestCRDensity>(TestCRDensity());
	
	auto intSynchro = std::make_shared<SynchroIntegrator>(
		SynchroIntegrator(mfield, crdensity));

	auto intAbsorp = std::make_shared<SynchroAbsorptionIntegrator>(
		SynchroAbsorptionIntegrator(mfield, crdensity, gdensity));

	QDirection direction;
        direction[0] = 90_deg;
        direction[1] = 180_deg;
	// dist = 11.5_kpc
	
	// 100_GHz => optical depth = 1e-5	
	QTemperature T_S = intSynchro->integrateOverLOS(direction, 100_GHz);
	QTemperature T_A = intAbsorp->integrateOverLOS(direction, 100_GHz);
	//EXPECT_NEAR(static_cast<double>(T_S), static_cast<double>(T_A), 1e-5); // K
	
	// 100_MHz => optical depth = 0.49
	// T_abs / T_sync = (1-exp(-depth))/depth \approx 0.79
	T_S = intSynchro->integrateOverLOS(direction, 100_MHz);
	T_A = intAbsorp->integrateOverLOS(direction, 100_MHz);
	//EXPECT_NEAR(static_cast<double>(T_A)/static_cast<double>(T_S), 0.79, 0.01);
	
	// 10_MHz => optical depth = 57.13
	// T_abs / T_sync = (1-exp(-depth))/depth \approx 0.79
	T_S = intSynchro->integrateOverLOS(direction, 10_MHz);
	T_A = intAbsorp->integrateOverLOS(direction, 10_MHz);
	//EXPECT_NEAR(static_cast<double>(T_A)/static_cast<double>(T_S), 0.017, 0.001);
}


int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


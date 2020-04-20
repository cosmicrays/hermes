#include "gtest/gtest.h"

#include "hermes.h"
#include <fstream>

namespace hermes {

/*
	Example 1 from cparamlib
	total inclusive gamma-ray cross section
	Tp = 512000.00 GeV
	E = 100.00 GeV
	=> dsigma/dlogE = 179.5 mb
*/
TEST(Interactions, cparamlib) {

	auto interaction = std::make_shared<interactions::Kamae06Gamma>(interactions::Kamae06Gamma());

	QEnergy E_p = 512000_GeV;
	QEnergy E_gamma = 100_GeV;
	QDifferentialCrossSection dsigma_dE = interaction->getDiffCrossSectionDirectly(E_p, E_gamma);
	
	QArea r = dsigma_dE * E_gamma;

	EXPECT_NEAR(
		static_cast<double>(r),
		static_cast<double>(179.5_mbarn),
		static_cast<double>(0.1_mbarn));
}

TEST(Interactions, KleinNishina) {

	auto interaction = std::make_shared<interactions::KleinNishina>(interactions::KleinNishina());

	// low-energy gamma limit
	double lorentz = 1e10;
	QEnergy Eelectron = lorentz * m_electron*c_squared;
	QEnergy Ephoton = 1.0e-3_eV;
	QEnergy Egamma = 1.0_eV;

	EXPECT_NEAR(static_cast<double>(
			interaction->getDiffCrossSection(Eelectron, Ephoton, Egamma)),
		    static_cast<double>(3/4. * sigma_Thompson / Ephoton / (lorentz*lorentz)),
		    1e-30);
	
	// high-energy gamma -> zero
	EXPECT_NEAR(static_cast<double>(
			interaction->getDiffCrossSection(Eelectron, Ephoton, 0.1*Eelectron)),
		    0.0,
		    1e-25);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


#include "gtest/gtest.h"

#include "hermes.h"

namespace hermes {

/*
	Example 1 from cparamlib
	total inclusive gamma-ray cross section
	Tp = 512000.00 GeV
	E = 100.00 GeV
	=> dsigma/dlogE = 179.5 mb
*/
TEST(Interactions, cparamlib) {

	auto interaction = std::make_shared<Kamae06>(Kamae06());

	QEnergy E_p = 512000_GeV;
	QEnergy E_gamma = 100_GeV;
	QDiffCrossSection dsigma_dE = interaction->getDiffCrossSection(E_p, E_gamma);
	
	QArea r = dsigma_dE * E_gamma;

	EXPECT_NEAR(
		static_cast<double>(r),
		static_cast<double>(179.5_mbarn),
		static_cast<double>(0.1_mbarn));
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


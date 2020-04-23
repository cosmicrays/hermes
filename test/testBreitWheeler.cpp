#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

TEST(Interactions, BreitWheeler) {
	auto bw = std::make_shared<interactions::BreitWheeler>(
	    interactions::BreitWheeler());

	QEnergy E_ph = 1_eV;
	QEnergy E_gamma = 300_TeV;
	QArea res = bw->integrateOverTheta(E_gamma, E_ph);
	EXPECT_NEAR(static_cast<double>(res), static_cast<double>(16.68_mbarn),
	            static_cast<double>(0.01_mbarn));

	E_ph = 1_eV;
	E_gamma = 50_TeV;
	res = bw->integrateOverTheta(E_gamma, E_ph);
	EXPECT_NEAR(static_cast<double>(res), static_cast<double>(72.24_mbarn),
	            static_cast<double>(0.01_mbarn));
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

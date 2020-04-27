#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

TEST(Interactions, BreitWheeler) {
	auto bw = std::make_shared<interactions::BreitWheeler>(
	    interactions::BreitWheeler());

	QEnergy E_ph = 1_eV;
	QEnergy E_gamma = 300_TeV;
	QArea res = bw->integratedOverTheta(E_gamma, E_ph);
	EXPECT_NEAR(static_cast<double>(res), static_cast<double>(16.68_mbarn),
	            static_cast<double>(0.01_mbarn));

	E_ph = 1_eV;
	E_gamma = 50_TeV;
	res = bw->integratedOverTheta(E_gamma, E_ph);
	EXPECT_NEAR(static_cast<double>(res), static_cast<double>(72.24_mbarn),
	            static_cast<double>(0.01_mbarn));
}

TEST(Interactions, BreitWheelerOverCMB) {
	auto bw = std::make_shared<interactions::BreitWheeler>();
	auto ph = std::make_shared<photonfields::CMB>();
	
	auto E_gamma = 50_TeV;

	for (auto itE = std::next(ph->begin()); itE != ph->end(); ++itE) {
		auto E_ph = (*itE);
		auto res = bw->integratedOverTheta(E_gamma, E_ph);
	}

}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

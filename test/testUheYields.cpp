#include <array>
#include <cmath>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

namespace {

struct ExpectedYield {
	interactions::UheHadronicModel model;
	double gammaXDNdX;
	double neutronXDNdX;
};

constexpr std::array<ExpectedYield, 3> expectedYieldsAtX01 = {
    ExpectedYield{interactions::UheHadronicModel::Sibyll23e, 0.461709277071307, 0.0593697227884274},
    ExpectedYield{interactions::UheHadronicModel::QGSJetII04, 0.368957098827514, 0.0786019661085087},
    ExpectedYield{interactions::UheHadronicModel::EposLHC, 0.438817916875609, 0.101512585007139},
};

template <typename CrossSection>
double recoveredXDNdX(const CrossSection &crossSection, QEnergy Eprojectile, QEnergy Esecondary) {
	return static_cast<double>(crossSection.getDiffCrossSection(Eprojectile, Esecondary) * Esecondary /
	                           interactions::UheYields::sigmaInelastic(Eprojectile));
}

void expectNearRelative(double actual, double expected, double tolerance) {
	EXPECT_NEAR(actual, expected, std::abs(expected) * tolerance);
}

}  // namespace

TEST(UheYieldsGamma, MatchesScalingFitAtX01) {
	const QEnergy Eprojectile = 1e10 * 1_GeV;
	const QEnergy Egamma = 1e9 * 1_GeV;

	for (const auto &expected : expectedYieldsAtX01) {
		SCOPED_TRACE(static_cast<int>(expected.model));
		const interactions::UheYieldsGamma gammaYields(expected.model);

		expectNearRelative(recoveredXDNdX(gammaYields, Eprojectile, Egamma), expected.gammaXDNdX, 1e-12);
	}
}

TEST(UheYieldsNeutrons, MatchesScalingFitAtX01) {
	const QEnergy Eprojectile = 1e10 * 1_GeV;
	const QEnergy Eneutron = 1e9 * 1_GeV;

	for (const auto &expected : expectedYieldsAtX01) {
		SCOPED_TRACE(static_cast<int>(expected.model));
		const interactions::UheYieldsNeutrons neutronYields(expected.model);

		expectNearRelative(recoveredXDNdX(neutronYields, Eprojectile, Eneutron), expected.neutronXDNdX, 1e-12);
	}
}

TEST(UheYields, ReturnsZeroForUnphysicalKinematics) {
	const interactions::UheYieldsGamma gammaYields;
	const interactions::UheYieldsNeutrons neutronYields;
	const QEnergy Eprojectile = 1e10 * 1_GeV;
	const QEnergy Esecondary = 2e10 * 1_GeV;

	EXPECT_DOUBLE_EQ(static_cast<double>(gammaYields.getDiffCrossSection(Eprojectile, Esecondary)), 0.);
	EXPECT_DOUBLE_EQ(static_cast<double>(neutronYields.getDiffCrossSection(Eprojectile, Esecondary)), 0.);
	EXPECT_DOUBLE_EQ(static_cast<double>(gammaYields.getDiffCrossSection(Eprojectile, QEnergy(0))), 0.);
	EXPECT_DOUBLE_EQ(static_cast<double>(neutronYields.getDiffCrossSection(QEnergy(0), 1e9 * 1_GeV)), 0.);
}

TEST(UheYields, EnforcesLowerFitBoundary) {
	const interactions::UheYieldsGamma gammaYields;
	const interactions::UheYieldsNeutrons neutronYields;
	const QEnergy Eprojectile = 1e10 * 1_GeV;

	EXPECT_DOUBLE_EQ(static_cast<double>(gammaYields.getDiffCrossSection(Eprojectile, 1e-3 * Eprojectile)), 0.);
	EXPECT_DOUBLE_EQ(static_cast<double>(neutronYields.getDiffCrossSection(Eprojectile, 1e-3 * Eprojectile)), 0.);
	EXPECT_GT(static_cast<double>(gammaYields.getDiffCrossSection(Eprojectile, 1.001e-3 * Eprojectile)), 0.);
	EXPECT_GT(static_cast<double>(neutronYields.getDiffCrossSection(Eprojectile, 1.001e-3 * Eprojectile)), 0.);
}

TEST(UheYields, AppliesNuclearScalingForPidOverload) {
	const interactions::UheYieldsGamma gammaYields;
	const interactions::UheYieldsNeutrons neutronYields;
	const QEnergy Eprojectile = 1e10 * 1_GeV;
	const QEnergy Esecondary = 1e9 * 1_GeV;

	const auto expectedScaling = interactions::nuclearScaling(Proton, Helium);
	const auto gammaBase = gammaYields.getDiffCrossSection(Eprojectile, Esecondary);
	const auto neutronBase = neutronYields.getDiffCrossSection(Eprojectile, Esecondary);

	EXPECT_NEAR(static_cast<double>(gammaYields.getDiffCrossSection(Proton, Helium, Eprojectile, Esecondary)),
	            static_cast<double>(expectedScaling * gammaBase), std::abs(static_cast<double>(gammaBase)) * 1e-12);
	EXPECT_NEAR(static_cast<double>(neutronYields.getDiffCrossSection(Proton, Helium, Eprojectile, Esecondary)),
	            static_cast<double>(expectedScaling * neutronBase), std::abs(static_cast<double>(neutronBase)) * 1e-12);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

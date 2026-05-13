#include <cmath>
#include <stdexcept>
#include <vector>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

namespace {

QPDensityPerEnergy equationNineDensity(QEnergy energy) {
	const QPDensityPerEnergy n0 = 1.748e-45 / 1_eV / 1_cm3;
	const QEnergy e0 = std::pow(10., 18.5) * 1_eV;
	const QEnergy e12 = 5e18 * 1_eV;
	const QEnergy e23 = 13e18 * 1_eV;
	const QEnergy e34 = 46e18 * 1_eV;
	constexpr double gamma1 = 3.29;
	constexpr double gamma2 = 2.51;
	constexpr double gamma3 = 3.05;
	constexpr double gamma4 = 5.1;
	constexpr double omega = 0.05;

	auto smoothBreak = [](QEnergy energy, QEnergy breakEnergy, double gammaBefore, double gammaAfter) {
		return std::pow(1.0 + std::pow(static_cast<double>(energy / breakEnergy), 1.0 / omega),
		                (gammaBefore - gammaAfter) * omega);
	};

	return n0 * std::pow(static_cast<double>(energy / e0), -gamma1) * smoothBreak(energy, e12, gamma1, gamma2) *
	       smoothBreak(energy, e23, gamma2, gamma3) * smoothBreak(energy, e34, gamma3, gamma4);
}

void expectNearRelative(QPDensityPerEnergy actual, QPDensityPerEnergy expected, double tolerance) {
	const double expectedValue = static_cast<double>(expected);
	EXPECT_NEAR(static_cast<double>(actual), expectedValue, std::abs(expectedValue) * tolerance);
}

}  // namespace

TEST(UHECR, BuildsLogarithmicEnergyAxis) {
	cosmicrays::UHECR uhecr(Proton, 1e18 * 1_eV, 1e21 * 1_eV, 4);
	const auto energyAxis = uhecr.getEnergyAxis();

	ASSERT_EQ(energyAxis.size(), 4);
	EXPECT_NEAR(static_cast<double>(energyAxis[0] / 1_eV), 1e18, 1e6);
	EXPECT_NEAR(static_cast<double>(energyAxis[1] / 1_eV), 1e19, 1e7);
	EXPECT_NEAR(static_cast<double>(energyAxis[2] / 1_eV), 1e20, 1e8);
	EXPECT_NEAR(static_cast<double>(energyAxis[3] / 1_eV), 1e21, 1e9);
	EXPECT_NEAR(uhecr.getEnergyScaleFactor(), 10., 1e-12);
}

TEST(UHECR, RejectsInvalidEnergyAxisParameters) {
	EXPECT_THROW(cosmicrays::UHECR(Proton, 1e18 * 1_eV, 1e21 * 1_eV, 1), std::invalid_argument);
	EXPECT_THROW(cosmicrays::UHECR(Proton, QEnergy(0), 1e21 * 1_eV, 4), std::invalid_argument);
	EXPECT_THROW(cosmicrays::UHECR(Proton, 1e18 * 1_eV, 1e18 * 1_eV, 4), std::invalid_argument);
	EXPECT_THROW(cosmicrays::UHECR(Proton, 1e21 * 1_eV, 1e18 * 1_eV, 4), std::invalid_argument);
}

TEST(UHECR, ReturnsZeroBelowSpectrumThreshold) {
	const cosmicrays::UHECR uhecr;
	const Vector3QLength position(0, 0, 0);

	EXPECT_DOUBLE_EQ(static_cast<double>(uhecr.getDensityPerEnergy(9.9e17 * 1_eV, position)), 0.);
	EXPECT_GT(static_cast<double>(uhecr.getDensityPerEnergy(1e18 * 1_eV, position)), 0.);
}

TEST(UHECR, MatchesEquationNineSpectrum) {
	const cosmicrays::UHECR uhecr;
	const Vector3QLength position(8.3_kpc, 0, 0);
	const std::vector<QEnergy> energies = {1e18 * 1_eV,  5e18 * 1_eV,  1e19 * 1_eV,
	                                       13e18 * 1_eV, 46e18 * 1_eV, 1e20 * 1_eV};

	for (const auto energy : energies) {
		SCOPED_TRACE(static_cast<double>(energy / 1_eV));
		expectNearRelative(uhecr.getDensityPerEnergy(energy, position), equationNineDensity(energy), 1e-12);
	}
}

TEST(UHECR, MatchesAugerPlotScale) {
	const cosmicrays::UHECR uhecr;
	const Vector3QLength position(8.3_kpc, 0, 0);
	const QEnergy energy = 3e19 * 1_eV;

	const QDiffIntensity intensity = uhecr.getDensityPerEnergy(energy, position) * c_light / (4_pi * 1_sr);
	const auto scaledIntensity = pow<3>(energy) * intensity;
	const double scaledIntensityInAugerUnits =
	    static_cast<double>(scaledIntensity / (pow<2>(1_eV) / (pow<2>(1_km) * year * 1_sr)));

	EXPECT_NEAR(scaledIntensityInAugerUnits, 6e37, 1e37);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

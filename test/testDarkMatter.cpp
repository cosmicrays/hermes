#include <chrono>
#include <memory>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

TEST(DarkMatterIntegrator, spectralEmissivity) {
	auto dmSpectrum = std::make_shared<darkmatter::PPPC4DMIDSpectrum>(
	    darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu,
	                                  darkmatter::Mass::m100GeV));

	QMass M_200 =
	    0.7 * 8e11 *
	    sun_mass;  // Battaglia et al., 2005, MNRAS, 364 (converted to M_200)
	double concentration = 18;  // Battaglia et al., 2005, MNRAS, 364
	double gamma_slope = 1.;    // NFW
	auto dmProfile = std::make_shared<darkmatter::NFW1996Profile>(
	    gamma_slope, concentration, M_200);

	auto i = std::make_shared<DarkMatterIntegrator>(dmSpectrum, dmProfile);

	auto units = 1. / 1_GeV / 1_cm3 / 1_s;  // sr!
	auto Egamma = 10_GeV;
	Vector3QLength v{8.1, 0, 0};
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

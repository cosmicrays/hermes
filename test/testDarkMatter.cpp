#include <chrono>
#include <memory>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

TEST(DarkMatter, PPPC4DMIDSpectrum) {
	auto dmSpectrum = std::make_shared<darkmatter::PPPC4DMIDSpectrum>(
	    darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu,
	                                  darkmatter::Mass::m100GeV));
	auto Egamma = 10_GeV;

	auto m = dmSpectrum->getRestMassEnergy();
	EXPECT_FLOAT_EQ(static_cast<double>(m), static_cast<double>(100_GeV));

	auto s = dmSpectrum->getParticlesPerEnergy(Egamma) * 1_erg;
	EXPECT_NEAR(static_cast<double>(s), 3.66656, 1e-4);
}

TEST(DarkMatter, NFWGProfile) {
	QMass M_200 =
	    0.7 * 8e11 *
	    sun_mass;  // Battaglia et al., 2005, MNRAS, 364 (converted to M_200)
	double concentration = 18;  // Battaglia et al., 2005, MNRAS, 364
	double gamma_slope = 1.;    // NFW
	auto dmProfile = std::make_shared<darkmatter::NFWGProfile>(
	    gamma_slope, concentration, M_200);

	Vector3QLength r{8.1_kpc, 0, 0};
	auto rho = dmProfile->getMassDensity(r.getR());
	EXPECT_NEAR(static_cast<double>(rho), 6.0315e-22, 1e-24);
}

TEST(DarkMatterIntegrator, spectralEmissivity) {
	auto dmSpectrum = std::make_shared<darkmatter::PPPC4DMIDSpectrum>(
	    darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu,
	                                  darkmatter::Mass::m100GeV));

	QMass M_200 =
	    0.7 * 8e11 *
	    sun_mass;  // Battaglia et al., 2005, MNRAS, 364 (converted to M_200)
	double concentration = 18;  // Battaglia et al., 2005, MNRAS, 364
	double gamma_slope = 1.;    // NFW
	auto dmProfile = std::make_shared<darkmatter::NFWGProfile>(
	    gamma_slope, concentration, M_200);

	auto i = std::make_shared<DarkMatterIntegrator>(dmSpectrum, dmProfile);

	auto units = 1. / 1_GeV / 1_cm3 / 1_s;  // sr!
	auto Egamma = 10_GeV;
	Vector3QLength r{8.1_kpc, 0, 0};

	auto e = i->spectralEmissivity(r, Egamma) / units;

	EXPECT_NEAR(static_cast<double>(e), 1e-33, 1e-34);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

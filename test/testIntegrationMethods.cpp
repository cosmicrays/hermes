#include "gtest/gtest.h"
#include <gsl/gsl_integration.h>
#include <memory>

#include "hermes.h"

namespace hermes {

// test integral f(x) = sin(x)/x
auto integrand = [](const QLength &dist) {
	return std::sin(static_cast<double>(dist)) / static_cast<double>(dist);
};
const double integral_result = 0.6241500516015882032;

TEST(IntegrationMethods, sumIntegration) {

	auto result = sumIntegration<QLength, double>(
	    [](QLength dist) { return integrand(dist); }, 1_m, 1000_m, 450000);

	EXPECT_NEAR(static_cast<double>(result), integral_result, 1e-3);
}

TEST(IntegrationMethods, trapesoidIntegration) {

	auto result = trapesoidIntegration<QLength, double>(
	    [](QLength dist) { return integrand(dist); }, 1_m, 1000_m, 20000);

	EXPECT_NEAR(static_cast<double>(result), integral_result, 1e-4);
}

TEST(IntegrationMethods, simpsonIntegration) {

	auto result = simpsonIntegration<QLength, double>(
	    [](QLength dist) { return integrand(dist); }, 1_m, 1000_m, 4000);

	EXPECT_NEAR(static_cast<double>(result), integral_result, 1e-5);
}

TEST(IntegrationMethods, gslQAGIntegration) {

	auto result = gslQAGIntegration<QLength, double>(
	    [](QLength dist) { return integrand(dist); }, 1_m, 1000_m, 500);

	EXPECT_NEAR(static_cast<double>(result), integral_result, 1e-7);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

} // namespace hermes

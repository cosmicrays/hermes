#include <gsl/gsl_integration.h>

#include <memory>

#include "gtest/gtest.h"
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

TEST(IntegrationMethods, SumUsesExactlyNSubintervals) {
	auto result = sumIntegration<QLength, double>(
	    [](QLength) { return 2.0; }, 0_m, 5_m, 10);
	EXPECT_DOUBLE_EQ(static_cast<double>(result), static_cast<double>(10_m));
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

TEST(IntegrationMethods, SimpsonIsExactForCubicPolynomial) {
	auto result = simpsonIntegration<QLength, double>(
	    [](QLength dist) {
		    const double x = static_cast<double>(dist / 1_m);
		    return x * x * x;
	    },
	    0_m, 2_m, 10);
	EXPECT_NEAR(static_cast<double>(result / 1_m), 4.0, 1e-13);
}

TEST(IntegrationMethods, gslQAGIntegration) {
	auto result = gslQAGIntegration<QLength, double>(
	    [](QLength dist) { return integrand(dist); }, 1_m, 1000_m, 500);

	EXPECT_NEAR(static_cast<double>(result), integral_result, 1e-7);
}

TEST(IntegrationMethods, RejectsInvalidSubintervalCountsAndRanges) {
	std::function<double(QLength)> constant = [](QLength) { return 1.0; };

	EXPECT_THROW((sumIntegration<QLength, double>(constant, 0_m, 1_m, 0)),
	             std::invalid_argument);
	EXPECT_THROW((trapesoidIntegration<QLength, double>(constant, 0_m, 1_m, -1)),
	             std::invalid_argument);
	EXPECT_THROW((simpsonIntegration<QLength, double>(constant, 0_m, 1_m, 3)),
	             std::invalid_argument);
	EXPECT_THROW((gaussIntegration<QLength, double>(constant, 0_m, 1_m, 0)),
	             std::invalid_argument);
	EXPECT_THROW((gslQAGIntegration<QLength, double>(constant, 0_m, 1_m, GSL_LIMIT + 1)),
	             std::invalid_argument);
	EXPECT_THROW((gslQAGSIntegration<QLength, double>(constant, 0_m, 1_m, 0)),
	             std::invalid_argument);
	EXPECT_THROW((adaptiveSimpsonIntegration<QLength, double>(constant, 0_m, 1_m,
	                                                         1e-6_m, 1)),
	             std::invalid_argument);
	EXPECT_THROW((sumIntegration<QLength, double>(constant, 2_m, 1_m, 10)),
	             std::invalid_argument);
}

TEST(IntegrationMethods, ZeroWidthRangeReturnsZero) {
	std::function<double(QLength)> constant = [](QLength) { return 1.0; };
	EXPECT_EQ((sumIntegration<QLength, double>(constant, 2_m, 2_m, 10)), 0_m);
	EXPECT_EQ((trapesoidIntegration<QLength, double>(constant, 2_m, 2_m, 10)), 0_m);
	EXPECT_EQ((simpsonIntegration<QLength, double>(constant, 2_m, 2_m, 10)), 0_m);
	EXPECT_EQ((gslQAGIntegration<QLength, double>(constant, 2_m, 2_m, 10)), 0_m);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

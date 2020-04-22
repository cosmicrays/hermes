#include "gtest/gtest.h"
#include <memory>

#include "hermes.h"
#include <cmath>

namespace hermes {

TEST(NumericalIntegration, simpsonIntegration) {
	auto integrand = [](const QLength &dist) {
		return sin(dist / 1_m * 1_rad);
	};
	QLength r = simpsonIntegration<QLength, QNumber>(
	    [integrand](QLength dist) { return integrand(dist); }, 0, 10, 100);
	EXPECT_NEAR(static_cast<double>(r), 2 * std::pow(std::sin(5), 2),
		    0.001);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

} // namespace hermes

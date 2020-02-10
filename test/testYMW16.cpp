#include "gtest/gtest.h"
#include <memory>

#include "hermes.h"

namespace hermes {

TEST(YMW16, compareValues) {

	auto gdensity = std::make_shared<YMW16>(YMW16());

	// values obtained from the original model
	EXPECT_NEAR(5417465.80,
			static_cast<double>(
				gdensity->getDensity(
					Vector3QLength(0,0,0))),
			0.01);
	
	EXPECT_NEAR(3670508.05,
			static_cast<double>(
				gdensity->getDensity(
					Vector3QLength(100_pc,-100_pc,0))),
			0.01);
	
	EXPECT_NEAR(175356.03,
			static_cast<double>(
				gdensity->getDensity(
					Vector3QLength(300_pc,0,0))),
			0.01);

}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


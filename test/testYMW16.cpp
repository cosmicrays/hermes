#include "gtest/gtest.h"
#include <memory>

#include "hermes.h"

namespace hermes {

TEST(YMW16, compareValues) {

    auto gdensity = std::make_shared<chargedgas::YMW16>(chargedgas::YMW16());

    // Values obtained from the original model
    // https://www.atnf.csiro.au/research/pulsar/ymw16/index.php
    // Change in coordinates:
    // x' = y
    // y' = -x
    // z' = z
    EXPECT_NEAR(
	static_cast<double>(5.41746580 / 1_cm3),
	static_cast<double>(gdensity->getDensity(Vector3QLength(0, 0, 0))), 1);
    EXPECT_NEAR(
	static_cast<double>(3.891344 / 1_cm3),
	static_cast<double>(gdensity->getDensity(Vector3QLength(0, 0, 15_pc))),
	1);
    EXPECT_NEAR(static_cast<double>(3.67050805 / 1_cm3),
		static_cast<double>(
		    gdensity->getDensity(Vector3QLength(100_pc, -100_pc, 0))),
		1);
    EXPECT_NEAR(
	static_cast<double>(0.17535603 / 1_cm3),
	static_cast<double>(gdensity->getDensity(Vector3QLength(300_pc, 0, 0))),
	1);
    EXPECT_NEAR(static_cast<double>(0.070453 / 1_cm3),
		static_cast<double>(gdensity->getDensity(
		    Vector3QLength(4321_pc, -1234_pc, -111_pc))),
		1);
    EXPECT_NEAR(static_cast<double>(0.008080 / 1_cm3),
		static_cast<double>(gdensity->getDensity(
		    Vector3QLength(-999_pc, 999_pc, 999_pc))),
		1);
    EXPECT_NEAR(static_cast<double>(0.001527 / 1_cm3),
		static_cast<double>(
		    gdensity->getDensity(Vector3QLength(0_pc, -10_kpc, 3_kpc))),
		1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

} // namespace hermes

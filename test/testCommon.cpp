#include "gtest/gtest.h"
#include <memory>

#include "hermes.h"

namespace hermes {

TEST(Common, toGalCoord) {
	auto gal_dir_1 = toGalCoord(QDirection({90_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(0_deg),
			static_cast<double>(gal_dir_1[0]));
	
	auto gal_dir_2 = toGalCoord(QDirection({179_deg, 270_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(-89_deg),
			static_cast<double>(gal_dir_2[0]));
	EXPECT_DOUBLE_EQ(static_cast<double>(270_deg),
			static_cast<double>(gal_dir_2[1]));
	
	auto gal_dir_3 = toGalCoord(QDirection({1_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(89_deg),
			static_cast<double>(gal_dir_3[0]));
	
	auto gal_dir_4 = toGalCoord(QDirection({100_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(-10_deg),
			static_cast<double>(gal_dir_4[0]));
	
	auto gal_dir_5 = toGalCoord(QDirection({200_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(70_deg),
			static_cast<double>(gal_dir_5[0]));
}

TEST(Common, fromGalCoord) {
	auto gal_dir_1 = fromGalCoord(QDirection({90_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(0_deg),
			static_cast<double>(gal_dir_1[0]));
	
	auto gal_dir_2 = fromGalCoord(QDirection({89_deg, 270_deg}));
	EXPECT_NEAR(static_cast<double>(1_deg),
			static_cast<double>(gal_dir_2[0]), 1e-8);
	EXPECT_DOUBLE_EQ(static_cast<double>(270_deg),
			static_cast<double>(gal_dir_2[1]));
	
	auto gal_dir_3 = fromGalCoord(QDirection({1_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(89_deg),
			static_cast<double>(gal_dir_3[0]));
	
	auto gal_dir_4 = fromGalCoord(QDirection({-10_deg, 10_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(100_deg),
			static_cast<double>(gal_dir_4[0]));
}

TEST(Common, galacticBorder) {
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	QDirection direction;

	// pointing towards the galactic centre
	direction[0] = 90_deg;
	direction[1] = 0_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(positionSun, direction, 20_kpc)),
			static_cast<double>(28.5_kpc), static_cast<double>(1_pc));
	// away from the galactic centre
	direction[0] = 90_deg;
	direction[1] = 180_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(positionSun, direction, 20_kpc)),
			static_cast<double>(11.5_kpc), static_cast<double>(1_pc));
	// left
	direction[0] = 90_deg;
	direction[1] = 90_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(positionSun, direction, 20_kpc)),
			static_cast<double>(18.103_kpc), static_cast<double>(10_pc));
	// right
	direction[0] = 90_deg;
	direction[1] = 270_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(positionSun, direction, 20_kpc)),
			static_cast<double>(18.103_kpc), static_cast<double>(10_pc));
	// the galactic north
	direction[0] = 0_deg;
	direction[1] = 0_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(positionSun, direction, 20_kpc)),
			static_cast<double>(5.0_kpc), static_cast<double>(10_pc));
		//spherical border: (18.103_kpc);
	// the galactic south
	direction[0] = 180_deg;
	direction[1] = 0_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(positionSun, direction, 20_kpc)),
			static_cast<double>(5.0_kpc), static_cast<double>(10_pc));
		//spherical border: (18.103_kpc);
}

TEST(Common, intensityToTemperature) {
	// T = I * c^2 / (2 * nu^2 * k_boltzmann)
	QIntensity intensity(1);
	QTemperature temp = intensityToTemperature(intensity, 1_Hz);
	EXPECT_NEAR(static_cast<double>(temp), 3.2548e39, 1e36);
}


int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


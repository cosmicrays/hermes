#include "gtest/gtest.h"
#include <memory>

#include "hermes.h"

namespace hermes {

TEST(Common, galacticBorder) {
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	QDirection direction;

	// pointing towards the galactic centre
	direction[0] = 90_deg;
	direction[1] = 0_deg;
	EXPECT_NEAR(distanceToGalBorder(positionSun, direction).getValue(),
			(28.5_kpc).getValue(), (1_pc).getValue());
	// away from the galactic centre
	direction[0] = 90_deg;
	direction[1] = 180_deg;
	EXPECT_NEAR(distanceToGalBorder(positionSun, direction).getValue(),
			(11.5_kpc).getValue(), (1_pc).getValue());
	// left
	direction[0] = 90_deg;
	direction[1] = 90_deg;
	EXPECT_NEAR(distanceToGalBorder(positionSun, direction).getValue(),
			(18.103_kpc).getValue(), (10_pc).getValue());
	// right
	direction[0] = 90_deg;
	direction[1] = 270_deg;
	EXPECT_NEAR(distanceToGalBorder(positionSun, direction).getValue(),
			(18.103_kpc).getValue(), (10_pc).getValue());
	// the galactic north
	direction[0] = 0_deg;
	direction[1] = 0_deg;
	EXPECT_NEAR(distanceToGalBorder(positionSun, direction).getValue(),
			(18.103_kpc).getValue(), (10_pc).getValue());
	// the galactic south
	direction[0] = 180_deg;
	direction[1] = 0_deg;
	EXPECT_NEAR(distanceToGalBorder(positionSun, direction).getValue(),
			(18.103_kpc).getValue(), (10_pc).getValue());
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


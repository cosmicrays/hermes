#include "gtest/gtest.h"
#include <memory>

#include "hermes.h"

namespace hermes {

TEST(Common, galacticBorder) {
	Vector3QLength positionSun(10_kpc, 0, 0);
	QDirection direction;

	direction[0] = 90_deg;
	direction[1] = 0_deg;
	EXPECT_NEAR(distanceToGalBorder(positionSun, direction).getValue(),
			(30_kpc).getValue(), (1_pc).getValue());
	direction[0] = -90_deg;
	direction[1] = 0_deg;
	EXPECT_NEAR(distanceToGalBorder(positionSun, direction).getValue(),
			(10_kpc).getValue(), (1_pc).getValue());
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


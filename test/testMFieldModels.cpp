#include "gtest/gtest.h"

#include "hermes.h"

namespace hermes {

TEST(Sun08, disk) {
	auto Sun08 = std::make_shared<Sun08Field>(Sun08Field());
	Sun08->setUseHalo(false);

	Vector3QMField B1 = Sun08->getField(Vector3QLength(0,2_kpc,0));
	Vector3QMField B2 = Sun08->getField(Vector3QLength(0,-2_kpc,0));

	EXPECT_DOUBLE_EQ(static_cast<double>(B1.x), static_cast<double>(-B2.x));	
	EXPECT_DOUBLE_EQ(static_cast<double>(B1.y), static_cast<double>(-B2.y));	
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


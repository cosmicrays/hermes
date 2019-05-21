#include "gtest/gtest.h"

#include "hermes.h"
#include <array>

namespace hermes {

TEST(RingModelDensity, RingBoundaries) {
	auto ringModel = RingModelDensity();

	std::vector<std::pair<QLength, QLength> > result;

	for (auto ring : ringModel) {
		result.push_back(ring->getBoundaries());
        }

	std::array<QLength, 12> b = {0_kpc, 2_kpc, 3_kpc, 4_kpc, 5_kpc, 6_kpc,
                7_kpc, 9_kpc, 12_kpc, 15_kpc, 18_kpc, 35_kpc};

	EXPECT_EQ(static_cast<double>(result[0].second),
			static_cast<double>(b[1]));	
	EXPECT_EQ(static_cast<double>(result[1].second),
			static_cast<double>(b[2]));	
	EXPECT_EQ(static_cast<double>(result[11].first),
			static_cast<double>(b[11]));	

	Vector3QLength pos(3.5_kpc, 0_kpc, 1_kpc);

	EXPECT_TRUE(ringModel[2]->isInside(pos));
	EXPECT_FALSE(ringModel[3]->isInside(pos));
}

TEST(RingModelDensity, RingValues) {
	auto ringModel = RingModelDensity();
	QDirection dir = {90_deg,1_deg};
	for (auto ring : ringModel) {
		std::cerr << "Index: " << ring->getIndex() << std::endl;
		std::cerr << ring->getColumnDensity(dir) << std::endl;
        }
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


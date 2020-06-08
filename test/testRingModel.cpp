#include <algorithm>
#include <array>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

class RingModel : public ::testing::Test {
  protected:
	// void SetUp() override { }
	// void TearDown() override {}

	neutralgas::RingModel ringModel_HI =
	    neutralgas::RingModel(neutralgas::GasType::HI);
};

TEST_F(RingModel, RingBoundaries) {
	std::vector<std::pair<QLength, QLength>> result;

	for (const auto &ring : ringModel_HI) {
		result.push_back(ring->getBoundaries());
	}

	std::array<QLength, 12> b = {0_kpc, 2_kpc, 3_kpc,  4_kpc,  5_kpc,  6_kpc,
	                             7_kpc, 9_kpc, 12_kpc, 15_kpc, 18_kpc, 35_kpc};

	EXPECT_EQ(static_cast<double>(result[0].second), static_cast<double>(b[1]));
	EXPECT_EQ(static_cast<double>(result[1].second), static_cast<double>(b[2]));
	EXPECT_EQ(static_cast<double>(result[11].first),
	          static_cast<double>(b[11]));

	EXPECT_TRUE(ringModel_HI[3]->isInside(Vector3QLength(0, 4.5_kpc, 3.0_kpc)));
	EXPECT_FALSE(ringModel_HI[3]->isInside(Vector3QLength(1.5_kpc, 2.5_kpc, 3.0_kpc)));
}

TEST_F(RingModel, RingSelection) {
	auto list = ringModel_HI.getEnabledRings();

	EXPECT_TRUE(
	    std::all_of(list.begin(), list.end(), [](bool i) { return i; }));

	ringModel_HI.setEnabledRings({false, true, false, true, false, true, false,
	                              true, true, true, true, false});
	list = ringModel_HI.getEnabledRings();
	EXPECT_FALSE(
	    std::all_of(list.begin(), list.end(), [](bool i) { return i; }));

	ringModel_HI.enableRingNo(2);
	list = ringModel_HI.getEnabledRings();
	EXPECT_TRUE(list[2]);

	ringModel_HI.disableRingNo(2);
	EXPECT_FALSE(ringModel_HI.isRingEnabled(2));

	EXPECT_THROW(ringModel_HI.disableRingNo(99), std::runtime_error);
}

TEST_F(RingModel, isInside) {
	Vector3QLength pos(3.5_kpc, 0_kpc, 1_kpc);

	EXPECT_TRUE(ringModel_HI[2]->isInside(pos));
	EXPECT_FALSE(ringModel_HI[3]->isInside(pos));
}

TEST_F(RingModel, RingValues) {
	neutralgas::RingModel ringModel_H2 =
	    neutralgas::RingModel(neutralgas::GasType::H2);

	QDirection dir = {90_deg, 5_deg};
	QColumnDensity col_HI(0);
	QColumnDensity col_H2(0);

	for (const auto &ring : ringModel_HI)
		col_HI += ring->getHIColumnDensity(dir);

	for (const auto &ring : ringModel_H2)
		col_H2 += ring->getH2ColumnDensity(dir);

	EXPECT_NEAR(static_cast<double>(col_HI), 2e26, 5e25);
	EXPECT_NEAR(static_cast<double>(col_H2), 6.5e26, 5e25);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

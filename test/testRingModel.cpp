#include "gtest/gtest.h"

#include "hermes.h"
#include <algorithm>
#include <array>

namespace hermes {

class RingModel : public ::testing::Test {
      protected:
	// void SetUp() override { }
	// void TearDown() override {}

	neutralgas::RingModel ringModel_HI =
	    neutralgas::RingModel(neutralgas::RingType::HI);
};

TEST_F(RingModel, RingBoundaries) {
	std::vector<std::pair<QLength, QLength>> result;

	for (auto ring : ringModel_HI) {
		result.push_back(ring->getBoundaries());
	}

	std::array<QLength, 12> b = {0_kpc,  2_kpc,  3_kpc,  4_kpc,
				     5_kpc,  6_kpc,  7_kpc,  9_kpc,
				     12_kpc, 15_kpc, 18_kpc, 35_kpc};

	EXPECT_EQ(static_cast<double>(result[0].second),
		  static_cast<double>(b[1]));
	EXPECT_EQ(static_cast<double>(result[1].second),
		  static_cast<double>(b[2]));
	EXPECT_EQ(static_cast<double>(result[11].first),
		  static_cast<double>(b[11]));
}

TEST_F(RingModel, RingSelection) {
	auto list = ringModel_HI.getEnabledRings();

	EXPECT_TRUE(
	    std::all_of(list.begin(), list.end(), [](bool i) { return i; }));

	ringModel_HI.setEnabledRings({false, true, false, true, false, true,
				      false, true, true, true, true, false});
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
	neutralgas::RingModel ringModel_CO =
	    neutralgas::RingModel(neutralgas::RingType::CO);

	QDirection dir = {90_deg, 5_deg};
	auto X0 = 1.8e20 / 1_cm2 / 1_K / 1_km * 1_s;
	QColumnDensity col_HI(0);
	QColumnDensity col_H2(0);

	for (auto ring : ringModel_HI)
		col_HI += ring->getHIColumnDensity(dir);

	for (auto ring : ringModel_CO)
		col_H2 += X0 * ring->getCOIntensity(dir);

	EXPECT_NEAR(static_cast<double>(col_HI), 2e26, 5e25);
	EXPECT_NEAR(static_cast<double>(col_H2), 3e26, 5e25);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

} // namespace hermes

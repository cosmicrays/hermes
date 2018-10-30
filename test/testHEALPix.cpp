#include "gtest/gtest.h"

#include "hermes.h"

namespace hermes {

TEST(HEALPix, consistency) {
	std::array<QAngle, 2> thetaphi;
	long nside = 4;
	long npix = nside2npix(nside);

	for(long ipix = 0; ipix < npix; ++ipix) {
		thetaphi = pix2ang_ring(nside, ipix);
		EXPECT_EQ(ang2pix_ring(nside, thetaphi), ipix);
	}
}

TEST(HEALPix, pix2ang_ring) {
	std::array<QAngle, 2> thetaphi;

	thetaphi = pix2ang_ring(16, 1440);
	EXPECT_NEAR(thetaphi[0].getValue(), 1.529, 0.001);
	EXPECT_EQ(thetaphi[1].getValue(), 0);

	thetaphi = pix2ang_ring(32, 0);
	EXPECT_NEAR(thetaphi[0].getValue(), 0.0255, 0.001);
	EXPECT_NEAR(thetaphi[1].getValue(), 0.7853, 0.001);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


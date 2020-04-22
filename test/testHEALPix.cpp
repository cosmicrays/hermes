#include "gtest/gtest.h"

#include "hermes.h"

namespace hermes {

TEST(HEALPix, consistency) {
    QDirection thetaphi;
    long nside = 128;
    long npix = nside2npix(nside);

    for (long ipix = 0; ipix < npix; ++ipix) {
	thetaphi = pix2ang_ring(nside, ipix);
	EXPECT_EQ(ang2pix_ring(nside, thetaphi), ipix);
    }
}

TEST(HEALPix, galacticCentre) {
    long nside = 2;

    // see fig. 3 in The HEALPix Primer
    QDirection thetaphi = {pi / 2, pi + 0.01};
    EXPECT_EQ(ang2pix_ring(nside, thetaphi), 24);

    thetaphi[0] = 0.7;
    thetaphi[1] = 2 * pi - 0.1;
    EXPECT_EQ(ang2pix_ring(nside, thetaphi), 11);
}

TEST(HEALPix, pix2ang_ring) {
    QDirection thetaphi;

    thetaphi = pix2ang_ring(16, 1440);
    EXPECT_NEAR(static_cast<double>(thetaphi[0]), 1.529, 0.001);
    EXPECT_EQ(static_cast<double>(thetaphi[1]), 0);

    thetaphi = pix2ang_ring(32, 0);
    EXPECT_NEAR(static_cast<double>(thetaphi[0]), 0.0255, 0.001);
    EXPECT_NEAR(static_cast<double>(thetaphi[1]), 0.7853, 0.001);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

} // namespace hermes

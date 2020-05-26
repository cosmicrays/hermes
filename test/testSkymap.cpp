#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

typedef SkymapTemplate<QNumber, QFrequency> SimpleSkymap;
typedef IntegratorTemplate<QNumber, QFrequency> SimpleIntegrator;

class DummyIntegrator : public SimpleIntegrator {
  public:
	DummyIntegrator(){};
	~DummyIntegrator(){};
	QNumber integrateOverLOS(const QDirection &direction) const override {
		QDirection galacticCentre = {90_deg, 0};
		QDirection galacticNorth = {0, 0};
		if (isWithinAngle(direction, galacticCentre, 20_deg)) return QNumber(1);
		if (isWithinAngle(direction, galacticNorth, 30_deg)) return QNumber(-1);
		return QNumber(0);
	};
	QNumber integrateOverLOS(const QDirection &direction,
	                         const QFrequency &f) const override {
		return QNumber(0);
	}
};

TEST(Skymap, resNsideNpixelsConvert) {
	int nside = 8;
	auto skymap = std::make_shared<SimpleSkymap>(SimpleSkymap(nside));
	skymap->getDescription();

	EXPECT_EQ(skymap->getRes(), 3);

	skymap->setRes(4);
	EXPECT_EQ(skymap->getNside(), 16);
	EXPECT_EQ(skymap->getNpix(), 3072);
}

TEST(Skymap, sizeCheck) {
	int nside = 256;
	auto skymap = std::make_shared<GammaSkymap>(GammaSkymap(nside, 1_GeV));

	// auto size = sizeof(skymap);
	// std::cerr << "SizeOf = " << sizeof(skymap) << " Length = " <<
	// skymap->size() << std::endl;

	EXPECT_EQ(skymap->size(), 12 * 256 * 256);
}

TEST(Skymap, computePixel) {
	int nside = 4;
	auto skymap = std::make_shared<SimpleSkymap>(SimpleSkymap(nside));
	auto integrator = std::make_shared<DummyIntegrator>(DummyIntegrator());

	QDirection galacticCentre = {90_deg, 0};
	long int gcPixel = ang2pix_ring(nside, galacticCentre);
	QDirection galacticNorth = {0, 0};
	long int gnPixel = ang2pix_ring(nside, galacticNorth);

	skymap->computePixel(gcPixel, integrator);
	skymap->computePixel(gnPixel, integrator);

	EXPECT_EQ(static_cast<double>(skymap->getPixel(gcPixel)), 1);
	EXPECT_EQ(static_cast<double>(skymap->getPixel(gnPixel)), -1);
}

TEST(SkymapMask, RectangularWindow) {
	int nside = 32;
	long int pixel_1, pixel_2, pixel_3;
	QDirection dir_1, dir_2, dir_3;
	auto skymap = std::make_shared<SimpleSkymap>(SimpleSkymap(nside));
	auto integrator = std::make_shared<DummyIntegrator>(DummyIntegrator());
	skymap->setIntegrator(integrator);

	auto GC_mask = std::make_shared<RectangularWindow>(
	    RectangularWindow({5_deg, -5_deg}, {355_deg, 5_deg}));
	skymap->setMask(GC_mask);
	skymap->compute();

	dir_1 = {0_deg, 0_deg};
	pixel_1 = ang2pix_ring(nside, fromGalCoord(dir_1));
	dir_2 = {10_deg, 5_deg};
	pixel_2 = ang2pix_ring(nside, fromGalCoord(dir_2));
	dir_3 = {-3_deg, 357_deg};
	pixel_3 = ang2pix_ring(nside, fromGalCoord(dir_3));

	EXPECT_NE(static_cast<double>(skymap->getPixel(pixel_1)), UNSEEN);
	EXPECT_EQ(static_cast<double>(skymap->getPixel(pixel_2)), UNSEEN);
	EXPECT_NE(static_cast<double>(skymap->getPixel(pixel_3)), UNSEEN);

	auto GP_mask = std::make_shared<RectangularWindow>(
	    RectangularWindow({5_deg, -5_deg}, {0_deg, 360_deg}));
	skymap->setMask(GP_mask);
	skymap->compute();

	dir_1 = {2_deg, 2_deg};
	pixel_1 = ang2pix_ring(nside, fromGalCoord(dir_1));
	dir_2 = {180_deg, 4_deg};
	pixel_2 = ang2pix_ring(nside, fromGalCoord(dir_2));
	dir_3 = {-45_deg, 300_deg};
	pixel_3 = ang2pix_ring(nside, fromGalCoord(dir_3));

	EXPECT_NE(static_cast<double>(skymap->getPixel(pixel_1)), UNSEEN);
	EXPECT_NE(static_cast<double>(skymap->getPixel(pixel_2)), UNSEEN);
	EXPECT_EQ(static_cast<double>(skymap->getPixel(pixel_3)), UNSEEN);

	auto XY_mask = std::make_shared<RectangularWindow>(
	    RectangularWindow({40_deg, -30_deg}, {30_deg, 60_deg}));
	XY_mask->getDescription();
	skymap->setMask(XY_mask);
	skymap->compute();

	dir_1 = {0_deg, 40_deg};
	pixel_1 = ang2pix_ring(nside, fromGalCoord(dir_1));
	dir_2 = {80_deg, 120_deg};
	pixel_2 = ang2pix_ring(nside, fromGalCoord(dir_2));

	EXPECT_NE(static_cast<double>(skymap->getPixel(pixel_1)), UNSEEN);
	EXPECT_EQ(static_cast<double>(skymap->getPixel(pixel_2)), UNSEEN);
}

TEST(SkymapMask, CircularWindow) {
	auto mask = std::make_shared<CircularWindow>(
	    CircularWindow(QDirection{-20_deg, 0_deg}, 15_deg));

	auto dir1 = QDirection({60_deg, 120_deg});
	auto dir2 = QDirection({-30_deg, 0_deg});

	EXPECT_FALSE(mask->isAllowed(fromGalCoord(dir1)));
	EXPECT_TRUE(mask->isAllowed(fromGalCoord(dir2)));
}

TEST(SkymapMask, CombinationOfMasks) {
	auto mask = std::make_shared<MaskList>(MaskList());
	auto mask_circle = std::make_shared<CircularWindow>(
	    CircularWindow(QDirection{0_deg, 30_deg}, 10_deg));
	auto mask_window = std::make_shared<RectangularWindow>(
	    RectangularWindow({20_deg, -20_deg}, {20_deg, 50_deg}));
	auto invert_circle = std::make_shared<InvertMask>(InvertMask(mask_circle));
	mask->addMask(invert_circle);
	mask->addMask(mask_window);

	auto dir1 = QDirection({0_deg, 30_deg});
	auto dir2 = QDirection({19_deg, 30_deg});
	auto dir3 = QDirection({35_deg, 0_deg});

	EXPECT_FALSE(mask->isAllowed(fromGalCoord(dir1)));
	EXPECT_TRUE(mask->isAllowed(fromGalCoord(dir2)));
	EXPECT_FALSE(mask->isAllowed(fromGalCoord(dir3)));
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

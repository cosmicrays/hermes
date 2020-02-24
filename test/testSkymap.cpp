#include "gtest/gtest.h"

#include "hermes.h"

namespace hermes {

typedef SkymapTemplate<QNumber, QFrequency> SimpleSkymap;

class DummyIntegrator: public IntegratorTemplate<QNumber, QFrequency> {
public:
        DummyIntegrator() {
	};
        ~DummyIntegrator() {
	};
        QNumber integrateOverLOS(QDirection direction) const {
	
		QDirection galacticCentre = {90_deg, 0};
		QDirection galacticNorth = {0, 0};

		if(isWithinAngle(direction, galacticCentre, 20_deg))
			return QNumber(1);
		if(isWithinAngle(direction, galacticNorth, 30_deg))
			return QNumber(-1);
		return QNumber(0);
	};
};

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
	int nside = 12;
	auto mask = std::make_shared<RectangularWindow>(RectangularWindow(
			QDirection({40_deg, 10_deg}), QDirection({-30_deg, 90_deg})));
	auto skymap = std::make_shared<SimpleSkymap>(SimpleSkymap(
					nside, mask));
	auto integrator = std::make_shared<DummyIntegrator>(DummyIntegrator());
	
	skymap->setIntegrator(integrator);
	skymap->compute();
	
	QDirection dir_1 = {0_deg, 15_deg};
	long int pixel_1 = ang2pix_ring(nside, fromGalCoord(dir_1));
	QDirection dir_2 = {80_deg, 120_deg};
	long int pixel_2 = ang2pix_ring(nside, fromGalCoord(dir_2));


	EXPECT_NE(static_cast<double>(skymap->getPixel(pixel_1)), UNSEEN);	
	EXPECT_EQ(static_cast<double>(skymap->getPixel(pixel_2)), UNSEEN);	
}

TEST(SkymapMask, CircularWindow) {
	int nside = 12;
	
	auto dir1 = QDirection({60_deg, 120_deg});
	auto dir2 = QDirection({-30_deg, 0_deg});

	auto mask = std::make_shared<CircularWindow>(CircularWindow(
			QDirection{-20_deg, 0_deg}, 15_deg));

	EXPECT_FALSE(mask->isAllowed(fromGalCoord(dir1)));
	EXPECT_TRUE(mask->isAllowed(fromGalCoord(dir2)));
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


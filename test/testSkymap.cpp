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

	//skymap->setIntegrator(integrator);
	skymap->computePixel(gcPixel, integrator);
	skymap->computePixel(gnPixel, integrator);
	
	EXPECT_EQ(static_cast<double>(skymap->getPixel(gcPixel)), 1);
	EXPECT_EQ(static_cast<double>(skymap->getPixel(gnPixel)), -1);
}

TEST(SkymapMask, RectangularWindow) {
	int nside = 12;
	auto mask = std::make_shared<RectangularWindow>(RectangularWindow(
			QAngle(40_deg), QAngle(-40_deg), QAngle(50_deg), QAngle(90_deg)));
	auto skymap = std::make_shared<SimpleSkymap>(SimpleSkymap(
					nside, mask));
	auto integrator = std::make_shared<DummyIntegrator>(DummyIntegrator());
	
	QDirection dir_1 = {0, 0};
	long int pixel_1 = ang2pix_ring(nside, dir_1);
	QDirection dir_2 = {90_deg, 60_deg};
	long int pixel_2 = ang2pix_ring(nside, dir_2);

	skymap->setIntegrator(integrator);
	skymap->compute();

	EXPECT_EQ(static_cast<double>(skymap->getPixel(pixel_1)), UNSEEN);	
	EXPECT_NE(static_cast<double>(skymap->getPixel(pixel_2)), UNSEEN);	
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


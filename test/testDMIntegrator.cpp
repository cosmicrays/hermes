#include "gtest/gtest.h"
#include <chrono>
#include <memory>

#include "hermes.h"

namespace hermes {

class TestChargedGasDensity: public ChargedGasDensity {
public:
        TestChargedGasDensity() : ChargedGasDensity(1e4_K) { }
        QPDensity getDensity(const Vector3QLength &pos) const {
		return 1.0 / 1_cm3; 
	}
};

TEST(DMIntegrator, convertToUnits) {

	auto skymap = std::make_shared<DMSkymap>(DMSkymap(4));
	auto gdensity = std::make_shared<TestChargedGasDensity>(
		TestChargedGasDensity());
	auto intDM = std::make_shared<DMIntegrator>(
		DMIntegrator(gdensity));

	skymap->computePixel(0, intDM);
	QDispersionMeasure pixel = skymap->getPixel(0);
	skymap->convertToUnits(kilometre/metre3, "km / m^3");
	QDispersionMeasure convertedPixel = skymap->getPixel(0);

	EXPECT_EQ(static_cast<double>(pixel * 
				(parsec/centimetre3)/(kilometre/metre3)),
		  static_cast<double>(convertedPixel));	
}

TEST(DMIntegrator, integrateOverLOS) {

	int nside = 32;
	auto skymap = std::make_shared<DMSkymap>(DMSkymap(nside));
	auto gdensity = std::make_shared<YMW16>(YMW16());
	auto intDM = std::make_shared<DMIntegrator>(
		DMIntegrator(gdensity));
	skymap->setIntegrator(intDM);
	QDirection dir;
	int ipix;

	// values taken from Fig.1 of the YMW16 paper (arXiv:1610.09448)
	dir = fromGalCoord(QDirection({0_deg, 15_deg}));
	ipix = ang2pix_ring(nside, dir);
	skymap->computePixel(ipix, intDM);
	EXPECT_NEAR(static_cast<double>(skymap->getPixel(ipix)),
		    static_cast<double>(3200*parsec/centimetre3),
		    static_cast<double>(200*parsec/centimetre3));
	dir = fromGalCoord(QDirection({0_deg, 170_deg}));
	ipix = ang2pix_ring(nside, dir);
	skymap->computePixel(ipix, intDM);
	EXPECT_NEAR(static_cast<double>(skymap->getPixel(ipix)),
		    static_cast<double>(250*parsec/centimetre3),
		    static_cast<double>(50*parsec/centimetre3));
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


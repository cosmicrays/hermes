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

	auto skymap = std::make_shared<DMSkymap>(DMSkymap(4));
	auto gdensity = std::make_shared<YMW16>(YMW16());
	auto intDM = std::make_shared<DMIntegrator>(
		DMIntegrator(gdensity));

	skymap->setIntegrator(intDM);
	skymap->compute();

	EXPECT_NEAR(static_cast<double>(skymap->operator[](0)),
		  11.548263939029292, 1e-3);	
}


int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


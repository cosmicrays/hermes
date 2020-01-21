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

	//skymap->setIntegrator(intDM);
	skymap->computePixel(0, intDM);
	QDispersionMeasure pixel = skymap->getPixel(0);
	skymap->convertToUnits(parsec/centimetre3);
	QDispersionMeasure convertedPixel = skymap->getPixel(0);

	EXPECT_EQ(static_cast<double>(pixel/parsec*centimetre3),
		  static_cast<double>(convertedPixel));	
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


#include "gtest/gtest.h"
#include <memory>

#include "hermes.h"

namespace hermes {

class TestMagneticField: public MagneticField {
        Vector3QMField value;
public:
        TestMagneticField() { }
        Vector3QMField getField(const Vector3QLength &pos) const {
		Vector3QLength pos_gc(0);
		Vector3QLength pos_sun(8.5_kpc, 0, 5_kpc);
		Vector3QLength pos_right(8.5_kpc, 5_kpc, 0);

		if ((pos-pos_gc).getR() < 1.5_kpc)
	        	return Vector3QMField(1_T, 1_T, 1_T);
		else if ((pos-pos_sun).getR() < 1.5_kpc)
			return Vector3QMField(-1_T, -1_T, -1_T);
		else if ((pos-pos_right).getR() < 1.5_kpc)
	        	return Vector3QMField(1_T, 1_T, 1_T);
		else
			return Vector3QMField(0);
        }
};

class TestChargedGasDensity: public ChargedGasDensity {
public:
        TestChargedGasDensity() { }
        QPDensity getDensity(const Vector3QLength &pos) const {
		return 1.0 / 1_cm3; 
	}
};

TEST(Integrator, MagneticField) {
	auto magfield = TestMagneticField();
	auto pos = Vector3QLength(0_kpc);
	EXPECT_EQ((magfield.getField(pos)).getX(), 1_T);
}

TEST(Integrator, Orientation) {
	QRotationMeasure pixel;
	auto magfield = std::make_shared<TestMagneticField>(TestMagneticField());
	auto gasdenisty = std::make_shared<TestChargedGasDensity>(TestChargedGasDensity());
	auto integrator = std::make_shared<RMIntegrator>(RMIntegrator(magfield, gasdenisty));
	auto skymap = std::make_shared<RMSkymap>(RMSkymap(4));
	QDirection direction;

	skymap->setIntegrator(integrator);
	skymap->compute();
	
	for (long ipix = 0; ipix < skymap->getSize(); ++ipix) {
		pixel = skymap->getPixel(ipix);
		direction = pix2ang_ring(4, ipix);
		
		// the galactic centre (theta ~ 90, phi ~ 0) should give != 0
		if ((fabs(direction[0] - 90_deg) <= 10_deg) && direction[1] == 0_deg)
			EXPECT_NE(pixel.getValue(), 0);
		// the galactic north
		else if (direction[0] <= 12_deg)
			EXPECT_LE(pixel.getValue(), 0);
		// right-hand side
		else if (fabs(direction[0] - 90_deg) <= 12_deg && fabs(direction[1] - 270_deg) < 12_deg)
			EXPECT_GE(pixel.getValue(), 0);
		else
			EXPECT_EQ(pixel.getValue(), 0);
	}
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


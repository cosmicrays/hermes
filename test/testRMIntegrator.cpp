#include "gtest/gtest.h"
#include <memory>

#include "hermes.h"

namespace hermes {

class TestMagneticField: public MagneticField {
        Vector3QMField value;
public:
        TestMagneticField() { }
        Vector3QMField getField(const Vector3QLength &pos) const {
		Vector3QLength pos_gc(0_pc, 0_pc, 0_pc);

		if (pos.getR() < 1.5_kpc)
	        	return Vector3QMField(1_T, 1_T, 1_T);
		else
			return Vector3QMField(0);
        }
};

class TestGasDensity: public GasDensity {
public:
        TestGasDensity() { }
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
	auto gasdenisty = std::make_shared<TestGasDensity>(TestGasDensity());
	auto integrator = std::make_shared<RMIntegrator>(RMIntegrator(magfield, gasdenisty));
	auto skymap = std::make_shared<RMSkymap>(RMSkymap(4));
	QDirection direction;

	skymap->setIntegrator(integrator);
	skymap->compute();

	for (long ipix = 0; ipix < skymap->getSize(); ++ipix) {
		pixel = skymap->getPixel(ipix);
		direction = pix2ang_ring(4, ipix);
		if (pixel.getValue() != 0)
			std::cerr << direction[0]/pi*180 << ", " << direction[1]/pi*180 << std::endl;
		// the galactic centre (theta ~ 90, phi ~ 0) should give != 0
		if (fabs(direction[0] - 90_deg) <= 10_deg && direction[1] == 0_deg)
			EXPECT_NE(pixel.getValue(), 0);
		else
			EXPECT_EQ(pixel.getValue(), 0);
	}
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


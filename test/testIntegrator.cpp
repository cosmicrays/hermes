#include "gtest/gtest.h"

#include "hermes.h"

namespace hermes {

class TestMagneticField: public MagneticField {
        Vector3MField value;
public:
        TestMagneticField() { }
        Vector3MField getField(const Vector3Length &pos) const {
		Vector3Length pos_gc(0);

		if (pos == pos_gc)
	        	value = 1*tesla;
		else
			value = 0*tesla;

		return value;
        }
};


TEST(Integrator, MF) {

	auto magfield = TestMagneticField();
	//EXPECT_NEAR(thetaphi[0].getValue(), 0.0255, 0.001);
	EXPECT((magfield.getR()).getValue(), 1*tesla);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


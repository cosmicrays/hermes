#include "gtest/gtest.h"

#include "hermes.h"

namespace hermes {

TEST(UnitsBasic, LengthOperations) {
	QLength x(2.3_km), y(3.2_m), z(0.0);
	z = x + y;
	EXPECT_EQ(z, 2303.2_m);
}

TEST(UnitsBasic, Angles) {
	QAngle omega(2_pi);

	EXPECT_EQ(omega, 360_deg);
	EXPECT_EQ(omega - 270_deg, pi/2.*radian);
}

TEST(UnitsDerived, Derived) {
	QEnergy E(10.0_J);
	QPressure P(5.0_Pa);
	EXPECT_EQ(E.getValue(), 10.0);
	EXPECT_EQ(P, 5 * newton / (metre*metre));
}

TEST(UnitsDerived, FineStructureConst) {
	QNumber alpha = mu0 / 4_pi * e_plus * e_plus * c_light / h_planck_bar;
	EXPECT_NEAR(alpha.getValue(), 1/137.035999, 0.0001);
}



int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes

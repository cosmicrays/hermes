#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

TEST(Picard3DTest, testDensities) {
	std::vector<PID> particleTypes = {Helium};
	auto cosmicRayModel = std::make_shared<cosmicrays::Picard3D>(particleTypes);
	double tolerance;
	double error = 1e-6;

	auto position = Vector3QLength(-20.0_kpc, -20.0_kpc, -4.0_kpc);
	int energy = 0;
	auto density = cosmicRayModel->getDensityPerEnergy(energy, position);
	double expected = 0.0000000000e+00;
	tolerance = expected * error;
	EXPECT_NEAR(static_cast<double>(density), expected, tolerance);

	position = Vector3QLength(17.5_kpc, 5.0_kpc, 3.5_kpc);
	energy = 20;
	density = cosmicRayModel->getDensityPerEnergy(energy, position);
	expected = 9.9185357000e-07;
	tolerance = expected * error;
	EXPECT_NEAR(static_cast<double>(density), expected, tolerance);

	position = Vector3QLength(0.0_kpc, 0.0_kpc, 0.0_kpc);
	energy = 30;
	density = cosmicRayModel->getDensityPerEnergy(energy, position);
	expected = 1.7621159598e-11;
	tolerance = expected * error;
	EXPECT_NEAR(static_cast<double>(density), expected, tolerance);

	position = Vector3QLength(-1.25_kpc, -1.25_kpc, 3.5_kpc);
	energy = 15;
	density = cosmicRayModel->getDensityPerEnergy(energy, position);
	expected = 1.5442656666e-01;
	tolerance = expected * error;
	EXPECT_NEAR(static_cast<double>(density), expected, tolerance);

	position = Vector3QLength(20.0_kpc, 20.0_kpc, 4.0_kpc);
	energy = 7;
	density = cosmicRayModel->getDensityPerEnergy(energy, position);
	expected = 0.0000000000e+00;
	tolerance = expected / 1e4;
	EXPECT_NEAR(static_cast<double>(density), expected, tolerance);
}

}  // namespace hermes

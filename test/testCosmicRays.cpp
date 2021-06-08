#include <chrono>
#include <memory>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

TEST(CosmicRays, Dragon2D) {
	std::vector<PID> particletypes = {Proton};
	auto cr_proton = std::make_shared<cosmicrays::Dragon2D>(
	    cosmicrays::Dragon2D(particletypes));
    Vector3QLength pos_obs(8.3_kpc, 0, 0);

    auto result = cr_proton->getDensityPerEnergy(0, pos_obs);
	
    EXPECT_NEAR(static_cast<double>(result),
	            static_cast<double>(3569822.7), 1);
}

TEST(CosmicRays, Dragon3D) {
	std::vector<PID> particletypes = {Proton};
	auto cr_proton = std::make_shared<cosmicrays::Dragon3D>(
	    cosmicrays::Dragon3D(particletypes));
    Vector3QLength pos_obs(8.3_kpc, 0, 0);

    auto result = cr_proton->getDensityPerEnergy(0, pos_obs);
	
    EXPECT_NEAR(static_cast<double>(result),
	            static_cast<double>(3217355.3), 1);
}


int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

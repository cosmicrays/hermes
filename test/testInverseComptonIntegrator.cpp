#include "gtest/gtest.h"
#include <memory>
#include <chrono>

#include "hermes.h"

namespace hermes {

TEST(InverseComptonIntegrator, integrateOverPhotonEnergy) {
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());
	auto photonField = std::make_shared<ISRF>(ISRF()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	
	Vector3QLength pos(0);
	QEnergy Egamma = 10_GeV;
	QEnergy Eelectron = 1_TeV;

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	auto res = intIC->integrateOverPhotonEnergy(pos, Egamma, Eelectron);
	std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
	
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	std::cerr << "res: " << res << std::endl;
	std::cerr << "Photon: " << milliseconds.count() << " ms" << std::endl;
	
	//EXPECT_NEAR(emissivity.getValue(), 3.915573e-55, 2e-56); // J/m^3
}

TEST(InverseComptonIntegrator, integrateOverEnergy) {
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());
	auto photonField = std::make_shared<ISRF>(ISRF()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	
	Vector3QLength pos(0);
	QEnergy Egamma = 1_GeV;

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	auto res = intIC->integrateOverEnergy(pos, Egamma);
	std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
	
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	std::cerr << "res: " << res << std::endl;
	std::cerr << "Electron: " << milliseconds.count() << " ms" << std::endl;
	
	//EXPECT_NEAR(emissivity.getValue(), 3.915573e-55, 2e-56); // J/m^3
}

TEST(InverseComptonIntegrator, integrateOverLOS) {
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());
	auto photonField = std::make_shared<ISRF>(ISRF()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	
	QDirection dir;
	dir[0] = 90_deg; dir[1] = 90_deg;
	QEnergy Egamma = 1_GeV;

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	auto res = intIC->integrateOverLOS(dir, Egamma);
	std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
	
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	std::cerr << "res: " << res << std::endl;
	std::cerr << "LOS: " << milliseconds.count() << " ms" << std::endl;
	
	//EXPECT_NEAR(emissivity.getValue(), 3.915573e-55, 2e-56); // J/m^3
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


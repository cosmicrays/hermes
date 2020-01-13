#include "gtest/gtest.h"
#include <memory>
#include <chrono>

#include "hermes.h"

namespace hermes {

TEST(InverseComptonIntegrator, integrateOverPhotonEnergy) {
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());
	auto photonField = std::make_shared<CMB>(CMB()); 
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
	auto photonField = std::make_shared<CMB>(CMB()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	
	Vector3QLength pos;
	pos.setX(30_pc);
	pos.setY(100_pc);
	pos.setZ(20_pc);
	QEnergy Egamma = 10_GeV;

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	auto res = intIC->integrateOverEnergy(pos, Egamma);
	std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
	
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	std::cerr << "res: " << res << std::endl;
	std::cerr << "Electron: " << milliseconds.count() << " ms" << std::endl;
	std::cerr << pos << std::endl;
	
	//EXPECT_NEAR(emissivity.getValue(), 3.915573e-55, 2e-56); // J/m^3
}

TEST(InverseComptonIntegrator, PerformanceTest) {
	std::vector<PID> particletypes = {Electron, Positron};
	auto dragonModel = std::make_shared<Dragon2DCRDensity>(Dragon2DCRDensity(
				getDataPath("CosmicRays/Gaggero17/run_2D.fits.gz"),
				particletypes)); 
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());
	auto photonField = std::make_shared<CMB>(CMB()); 
	auto in = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(dragonModel, photonField, kleinnishina));
	auto skymap = std::make_shared<GammaSkymap>(GammaSkymap(4, 1_GeV));
	skymap->setIntegrator(in);

        std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	skymap->compute();
        std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	float pxl_speed = milliseconds.count()/skymap->getNpix()*skymap->getThreadsNumber();
	
	std::cerr << "pxl spd: " << pxl_speed << " ms" << std::endl;
	
	EXPECT_LE(pxl_speed, 350); // ms
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


#include "gtest/gtest.h"
#include <memory>
#include <chrono>

#include "hermes.h"

namespace hermes {

/* Analytical integral over photonfields::CMB with constant cross-section */
TEST(InverseComptonIntegrator, integrateOverPhotonEnergyCMB) {
	auto simpleModel = std::make_shared<cosmicrays::SimpleCRDensity>(cosmicrays::SimpleCRDensity());
	auto dummyCS = std::make_shared<interactions::DummyCrossSection>(interactions::DummyCrossSection(1));
	auto photonField = std::make_shared<photonfields::CMB>(photonfields::CMB());
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, dummyCS));

	QEnergy Egamma = 10_GeV;
        QEnergy Eelectron = 1_TeV;

	auto res = intIC->integrateOverPhotonEnergy(Vector3QLength(0), Egamma, Eelectron);
	
	QTemperature T_CMB = 2.725_K;
	QPDensity analytical_res = 16_pi * pow<3>(k_boltzmann * T_CMB/(c_light * h_planck)) 
			 * 1.20206; // Zeta_f(3) = 1.20206
	// 410 photons/cm3
	EXPECT_NEAR(static_cast<double>(res * 1_cm3),
			static_cast<double>(analytical_res * 1_cm3), 1);
}

/* Integral over photon field energy with Klein-Nishina */
TEST(InverseComptonIntegrator, integrateOverPhotonEnergy) {
	auto simpleModel = std::make_shared<cosmicrays::SimpleCRDensity>(cosmicrays::SimpleCRDensity());
	auto kleinnishina = std::make_shared<interactions::KleinNishina>(interactions::KleinNishina());
	auto photonField = std::make_shared<photonfields::CMB>(photonfields::CMB()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));

	QEnergy Egamma = 0.1_GeV;
        QEnergy Eelectron = 0.2_TeV;
	
	QTemperature T_CMB = 2.725_K;
	QDifferentialCrossSection sigma(1e-17); // in m^2/J (a bit overestimate from K-N)
	auto analytical_res = sigma * 410 * 1e6; // from the previous test
	auto res = intIC->integrateOverPhotonEnergy(Vector3QLength(0), Egamma, Eelectron);
	
	EXPECT_LE(static_cast<double>(res),
			static_cast<double>(analytical_res));
	EXPECT_GE(static_cast<double>(res),
			static_cast<double>(0.1*analytical_res));
}

/* Take the result from above and insert it into an integral with the constant (dummy) CR flux */
TEST(InverseComptonIntegrator, integrateOverEnergy) {
	auto dummyModel = std::make_shared<cosmicrays::DummyCRDensity>(cosmicrays::DummyCRDensity(Electron));
	auto kleinnishina = std::make_shared<interactions::KleinNishina>(interactions::KleinNishina());
	auto photonField = std::make_shared<photonfields::CMB>(photonfields::CMB()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(dummyModel, photonField, kleinnishina));

	dummyModel->setScaleFactor(false);
	EXPECT_FALSE(dummyModel->existsScaleFactor());
	
	QEnergy Egamma = 1.0_GeV;
	auto emissivity = intIC->integrateOverEnergy(Vector3QLength(0), Egamma);
	// sigma_KN = ~230 mbarn for Ephoton(T_photonfields::CMB) and Egamma = 1 TeV

	// 5e-16 is calculated independently by integrating the integrateOverPhotonEnergy(E_el)
	EXPECT_NEAR(static_cast<double>(emissivity),
			static_cast<double>(5e-16*c_light/4_pi), 1e-7);
}

/* Check consistency of different integration methods */
TEST(InverseComptonIntegrator, compareLOSIntegrations) {
	auto simpleModel = std::make_shared<cosmicrays::SimpleCRDensity>(cosmicrays::SimpleCRDensity());
	auto kleinnishina = std::make_shared<interactions::KleinNishina>(interactions::KleinNishina());
	auto photonField = std::make_shared<photonfields::CMB>(photonfields::CMB()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	
	QDirection dir = {pi/2*1_rad, 0_rad};
	QLength maxDist = intIC->getMaxDistance(dir);
	QEnergy Egamma = 10_GeV;

	auto integrand = [intIC, dir, Egamma](const QLength &dist) {
		return intIC->integrateOverEnergy(
				getGalacticPosition(intIC->getSunPosition(), dist, dir),
				Egamma
			); };
	
	auto result_QAG = gslQAGIntegration<QDifferentialFlux, QGREmissivity>(
                        [maxDist, dir, integrand](QLength dist) {return integrand(dist);}, 0, maxDist, 300);
	
	auto result_SI  = simpsonIntegration<QDifferentialFlux, QGREmissivity>(
                        [maxDist, dir, integrand](QLength dist) {return integrand(dist);}, 0, maxDist, 300);


	EXPECT_NEAR(static_cast<double>(result_QAG),
			static_cast<double>(result_SI),
			1e-5);
}

/*
TEST(InverseComptonIntegrator, integrateOverLOS) {
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<interactions::KleinNishina>(interactions::KleinNishina());
	auto photonField = std::make_shared<photonfields::CMB>(photonfields::CMB()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	
	QDirection dir = {pi/2*1_rad, 0_rad};
	QLength maxDist = intIC->getMaxDistance(dir);
	QEnergy Egamma = 10_GeV;

	auto integrand = [intIC, dir, Egamma](const QLength &dist) {
		return intIC->integrateOverEnergy(
				getGalacticPosition(intIC->getSunPosition(), dist, dir),
				Egamma
			); };
	
	std::shared_ptr<gsl_integration_workspace> workspace_ptr;
	workspace_ptr = static_cast<std::shared_ptr<gsl_integration_workspace>>(
			gsl_integration_workspace_alloc(1000));

	auto result_QAG = gslQAGIntegration<QDifferentialFlux, QGREmissivity>(
                        [maxDist, dir, integrand](QLength dist) {return integrand(dist);}, 0, maxDist, 300, workspace_ptr);
	
	auto result_SI  = simpsonIntegration<QDifferentialFlux, QGREmissivity>(
                        [maxDist, dir, integrand](QLength dist) {return integrand(dist);}, 0, maxDist, 300);


	EXPECT_NEAR(static_cast<double>(result_QAG),
			static_cast<double>(result_SI),
			1e-5);
}*/

TEST(InverseComptonIntegrator, initCacheTable) {
	auto simpleModel = std::make_shared<cosmicrays::SimpleCRDensity>(cosmicrays::SimpleCRDensity());
	auto kleinnishina = std::make_shared<interactions::KleinNishina>(interactions::KleinNishina());
	auto photonField = std::make_shared<photonfields::ISRF>(photonfields::ISRF()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	
	QEnergy Egamma = 10_GeV;
	Vector3QLength pos1(0);
	Vector3QLength pos2(1_kpc, 1_kpc, 0.5_kpc);
	
	auto res1_withoutcache = intIC->integrateOverEnergy(pos1, Egamma);
	auto res2_withoutcache = intIC->integrateOverEnergy(pos2, Egamma);

	EXPECT_FALSE(intIC->isCacheTableEnabled());
	intIC->setupCacheTable(10, 10, 2);
	EXPECT_TRUE(intIC->isCacheTableEnabled());
	
	auto res1_withcache = intIC->integrateOverEnergy(pos1, Egamma);
	auto res2_withcache = intIC->integrateOverEnergy(pos2, Egamma);
	
	EXPECT_NEAR(static_cast<double>(res1_withoutcache),
			static_cast<double>(res1_withcache), 10);
	EXPECT_NEAR(static_cast<double>(res2_withoutcache),
			static_cast<double>(res2_withcache), 10);
}

TEST(InverseComptonIntegrator, GammaSkymapRange) {
	auto gammaskymap_range = std::make_shared<GammaSkymapRange>(GammaSkymapRange(4, 1_GeV, 100_TeV, 10));

	auto simpleModel = std::make_shared<cosmicrays::SimpleCRDensity>(cosmicrays::SimpleCRDensity());
	auto kleinnishina = std::make_shared<interactions::KleinNishina>(interactions::KleinNishina());
	auto photonField = std::make_shared<photonfields::CMB>(photonfields::CMB());

	auto in = std::make_shared<InverseComptonIntegrator>(
                InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	in->setupCacheTable(10, 10, 2);

	gammaskymap_range->setIntegrator(in);
	gammaskymap_range->compute();
	
	auto output = std::make_shared<outputs::FITSOutput>(outputs::FITSOutput("!InverseComptonIntegrator-GammaSkymapRange-output.fits.gz"));
	gammaskymap_range->save(output);
}	

TEST(InverseComptonIntegrator, PerformanceTest) {
	std::vector<PID> particletypes = {Electron, Positron};
	auto dragonModel = std::make_shared<cosmicrays::Dragon2DCRDensity>(cosmicrays::Dragon2DCRDensity(particletypes)); 
	auto simpleModel = std::make_shared<cosmicrays::SimpleCRDensity>(cosmicrays::SimpleCRDensity());
	auto kleinnishina = std::make_shared<interactions::KleinNishina>(interactions::KleinNishina());
	auto photonField = std::make_shared<photonfields::ISRF>(photonfields::ISRF()); 
	auto in = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(dragonModel, photonField, kleinnishina));
	auto Egamma = 1_GeV;
	auto skymap = std::make_shared<GammaSkymap>(GammaSkymap(4, Egamma));
	skymap->setIntegrator(in);
	
	in->setupCacheTable(10, 10, 2);

        std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	skymap->compute();
        std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	float pxl_speed = milliseconds.count()/skymap->getNpix()*getThreadsNumber();
	
	std::cerr << "pxl spd: " << pxl_speed << " ms" << std::endl;
	
	EXPECT_LE(pxl_speed, 1000); // ms
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


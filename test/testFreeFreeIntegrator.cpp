#include <chrono>
#include <memory>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

class TestIonizedGasDensity : public ionizedgas::IonizedGasDensity {
  public:
	TestIonizedGasDensity() : ionizedgas::IonizedGasDensity(1e4_K) {}
	QPDensity getDensity(const Vector3QLength &pos) const { return 1.0 / 1_cm3; }
};

TEST(FreeFreeIntegrator, gauntFactor) {
	auto gdensity = std::make_shared<TestIonizedGasDensity>(TestIonizedGasDensity());
	auto intFreeFree = std::make_shared<FreeFreeIntegrator>(FreeFreeIntegrator(gdensity));

	// see arXiv:1407.5048
	int Z = 1;
	auto gammaSquared = [](QTemperature T, int Z) { return Z * Z * Ry / (k_boltzmann * T); };
	auto u = [](QFrequency nu, QTemperature T) { return h_planck * nu / (k_boltzmann * T); };

	// std::cerr << gammaSquared(1e4_K, Z) << std::endl;
	// std::cerr << u(1_GHz, 1e4_K) << std::endl;
	/*
	    EXPECT_NEAR(static_cast<double>(intFreeFree->gauntFactor(1_GHz,
	   1e4_K, Z)), 5.6, 1e-1);
	    EXPECT_NEAR(static_cast<double>(intFreeFree->gauntFactor(1_MHz,
	   1000_K, Z)), 8.4506, 1e-4);
	    EXPECT_NEAR(static_cast<double>(intFreeFree->gauntFactor(50_MHz,
	   500_K, Z)), 5.7206, 1e-4);
	    EXPECT_NEAR(static_cast<double>(intFreeFree->gauntFactor(22_GHz,
	   2e4_K, Z)), 5.4154, 1e-4);
	*/
}

TEST(FreeFreeIntegrator, spectralEmissivityExplicit) {
	auto gdensity = std::make_shared<TestIonizedGasDensity>(TestIonizedGasDensity());
	auto intFreeFree = std::make_shared<FreeFreeIntegrator>(FreeFreeIntegrator(gdensity));
	/*
	    auto N = 1.0/1_cm3;
	    EXPECT_NEAR(static_cast<double>(
	        intFreeFree->spectralEmissivityExplicit(N, N, 10_MHz,
	   1e4_K, 1)), 6.2158e-40, 1e-41);

	    N = 1.0/1_cm3;
	    EXPECT_NEAR(static_cast<double>(
	        intFreeFree->spectralEmissivityExplicit(N, N, 1_GHz,
	   1e4_K, 1)), 4.4472e-40, 1e-41);

	    N = 1.0/1_cm3;
	    EXPECT_NEAR(static_cast<double>(
	        intFreeFree->spectralEmissivityExplicit(N, N, 22_GHz,
	   1e4_K, 1)), 3.3124e-40, 1e-42);

	    N = 10.0/1_cm3;
	    EXPECT_NEAR(static_cast<double>(
	        intFreeFree->spectralEmissivityExplicit(N, N, 1000_GHz,
	   1e4_K, 1)), 1.8642e-38, 1e-42);
	*/
}

TEST(FreeFreeIntegrator, integrateSkymap) {
	auto gdensity = std::make_shared<TestIonizedGasDensity>(TestIonizedGasDensity());
	auto intFreeFree = std::make_shared<FreeFreeIntegrator>(FreeFreeIntegrator(gdensity));

	QFrequency f = 22_GHz;
	auto skymap = std::make_shared<RadioSkymap>(RadioSkymap(2, f));
	skymap->setIntegrator(intFreeFree);
	skymap->compute();
	/*
	    QEmissivity em(3.3124e-40); // see the test above for f=22_GHz
	    // for spherical boundary:
	    // QTemperature minT = intensityToTemperature(em * 11.5_kpc /
	   4_pi, f); QTemperature minT = intensityToTemperature(em * 5_kpc /
	   4_pi, f); QTemperature maxT = intensityToTemperature(em * 28.5_kpc /
	   4_pi, f);

	    QTemperature pixel;
	    for (long ipix = 0; ipix < skymap->getSize(); ++ipix) {
	        pixel = skymap->getPixel(ipix);
	        EXPECT_GE(static_cast<double>(pixel),
	   static_cast<double>(minT)); EXPECT_LE(static_cast<double>(pixel),
	   static_cast<double>(maxT));
	    }
	*/
}

TEST(FreeFreeIntegrator, RadioSkymapRange) {
	auto gdensity = std::make_shared<TestIonizedGasDensity>(TestIonizedGasDensity());
	auto intFreeFree = std::make_shared<FreeFreeIntegrator>(FreeFreeIntegrator(gdensity));

	auto skymap_range = std::make_shared<RadioSkymapRange>(RadioSkymapRange(4, 1_MHz, 100_GHz, 10));
	skymap_range->setIntegrator(intFreeFree);
	skymap_range->compute();

	auto output = std::make_shared<outputs::HEALPixFormat>(
	    outputs::HEALPixFormat("!FreeFreeIntegrator-RadioSkymapRange-output.fits.gz"));
	skymap_range->save(output);
}

TEST(FreeFreeIntegrator, absorptionCoefficient) {
	auto gdensity = std::make_shared<TestIonizedGasDensity>(TestIonizedGasDensity());
	auto integrator = std::make_shared<FreeFreeIntegrator>(FreeFreeIntegrator(gdensity));

	/*
	    QInverseLength absorption = integrator->absorptionCoefficient(
	            Vector3QLength(1_pc,0,0), 10_MHz);
	    EXPECT_NEAR(static_cast<double>(absorption), 1.61e-19, 1e-21);
	   // 1/m

	    absorption = integrator->absorptionCoefficient(
	            Vector3QLength(1_pc,0,0), 1_GHz);
	    EXPECT_NEAR(static_cast<double>(absorption), 1.1512e-23, 1e-25);
	   // 1/m
	*/
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

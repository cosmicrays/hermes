#include <chrono>
#include <memory>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

void print_pixel_time(unsigned long t_pxl) {
	std::cerr << 
        "Average pixel run time (100 pixels)" <<
        std::endl <<
        "t_pxl * 100 = " <<
        t_pxl * 100 <<
        " ms" <<
        std::endl;
}

TEST(PerformanceTest, DispersionMeasureIntegrator) {
    int nside = 16;
	auto skymap = std::make_shared<DispersionMeasureSkymap>(DispersionMeasureSkymap(nside));
	auto gdensity = std::make_shared<ionizedgas::YMW16>(ionizedgas::YMW16());
	auto intDM = std::make_shared<DispersionMeasureIntegrator>(DispersionMeasureIntegrator(gdensity));
	skymap->setIntegrator(intDM);
	
    std::chrono::time_point<std::chrono::system_clock> start =
	    std::chrono::system_clock::now();
	skymap->compute();
	std::chrono::time_point<std::chrono::system_clock> stop =
	    std::chrono::system_clock::now();

	auto milliseconds =
	    std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	unsigned long pxl_speed =
	    milliseconds.count() / skymap->getNpix() * getThreadsNumber();

    print_pixel_time(pxl_speed);

	EXPECT_LE(pxl_speed, 1);  // ms
}

TEST(PerformanceTest, RotationMeasureIntegrator) {
    int nside = 8;
	auto magfield = std::make_shared<magneticfields::JF12>(magneticfields::JF12());
	auto gasdensity = std::make_shared<ionizedgas::YMW16>(ionizedgas::YMW16());
	auto in = std::make_shared<RotationMeasureIntegrator>(RotationMeasureIntegrator(magfield, gasdensity));
	auto skymap = std::make_shared<RotationMeasureSkymap>(RotationMeasureSkymap(nside));
	skymap->setIntegrator(in);

	std::chrono::time_point<std::chrono::system_clock> start =
        std::chrono::system_clock::now();
	skymap->compute();
	std::chrono::time_point<std::chrono::system_clock> stop =
        std::chrono::system_clock::now();

	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	unsigned long pxl_speed = milliseconds.count() / skymap->getNpix() * getThreadsNumber();
	
    print_pixel_time(pxl_speed);

	EXPECT_LE(pxl_speed, 45);  // ms
}

TEST(PerformanceTest, FreeFreeIntegrator) {
    int nside = 16;
	auto gasdensity = std::make_shared<ionizedgas::YMW16>(ionizedgas::YMW16());
	auto in = std::make_shared<FreeFreeIntegrator>(FreeFreeIntegrator(gasdensity));
	auto skymap = std::make_shared<RadioSkymap>(RadioSkymap(nside, 100_MHz));
	skymap->setIntegrator(in);

	std::chrono::time_point<std::chrono::system_clock> start =
        std::chrono::system_clock::now();
	skymap->compute();
	std::chrono::time_point<std::chrono::system_clock> stop =
        std::chrono::system_clock::now();

	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	unsigned long pxl_speed = milliseconds.count() / skymap->getNpix() * getThreadsNumber();

    print_pixel_time(pxl_speed);

	EXPECT_LE(pxl_speed, 15);  // ms
}


TEST(PerformanceTest, SynchroIntegrator) {
    int nside = 4;
	auto mfield = std::make_shared<magneticfields::JF12>(
	    magneticfields::JF12());
	std::vector<PID> particletypes = {Electron, Positron};
	auto dragonModel = std::make_shared<cosmicrays::Dragon2D>(
	    cosmicrays::Dragon2D(particletypes));
	auto in = std::make_shared<SynchroIntegrator>(
	    SynchroIntegrator(mfield, dragonModel));
	auto skymap = std::make_shared<RadioSkymap>(RadioSkymap(nside, 1_GHz));
	skymap->setIntegrator(in);

	std::chrono::time_point<std::chrono::system_clock> start =
	    std::chrono::system_clock::now();
	skymap->compute();
	std::chrono::time_point<std::chrono::system_clock> stop =
	    std::chrono::system_clock::now();

	auto milliseconds =
	    std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	unsigned long pxl_speed =
	    milliseconds.count() / skymap->getNpix() * getThreadsNumber();
    
    print_pixel_time(pxl_speed);

	EXPECT_LE(pxl_speed, 200);  // ms
}


TEST(PerformanceTest, PiZeroIntegrator) {
    int nside = 4;
	std::vector<PID> particletypes = {Proton};
	auto dragonModel = std::make_shared<cosmicrays::Dragon2D>(
	    cosmicrays::Dragon2D(particletypes));
	auto kamae = std::make_shared<interactions::Kamae06Gamma>(
	    interactions::Kamae06Gamma());
	auto ringModel = std::make_shared<neutralgas::RingModel>(
	    neutralgas::RingModel(neutralgas::GasType::H2));
	auto in = std::make_shared<PiZeroIntegrator>(
	    PiZeroIntegrator(dragonModel, ringModel, kamae));
	auto skymap = std::make_shared<GammaSkymap>(GammaSkymap(nside, 1_GeV));
	skymap->setIntegrator(in);
	
    in->setupCacheTable(10, 10, 2);

	std::chrono::time_point<std::chrono::system_clock> start =
	    std::chrono::system_clock::now();
	skymap->compute();
	std::chrono::time_point<std::chrono::system_clock> stop =
	    std::chrono::system_clock::now();

	auto milliseconds =
	    std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	unsigned long pxl_speed =
	    milliseconds.count() / skymap->getNpix() * getThreadsNumber();

    print_pixel_time(pxl_speed);

	EXPECT_LE(pxl_speed, 250);  // ms
}

TEST(PerformanceTest, InverseComptonIntegrator) {
    int nside = 4;
	std::vector<PID> particletypes = {Electron, Positron};
	auto dragonModel = std::make_shared<cosmicrays::Dragon2D>(
	    cosmicrays::Dragon2D(particletypes));
	auto kleinnishina = std::make_shared<interactions::KleinNishina>(
	    interactions::KleinNishina());
	auto photonField =
	    std::make_shared<photonfields::ISRF>(photonfields::ISRF());
	auto in = std::make_shared<InverseComptonIntegrator>(
	    InverseComptonIntegrator(dragonModel, photonField, kleinnishina));
	auto Egamma = 1_GeV;
	auto skymap = std::make_shared<GammaSkymap>(GammaSkymap(nside, Egamma));
	skymap->setIntegrator(in);

	in->setupCacheTable(10, 10, 2);

	std::chrono::time_point<std::chrono::system_clock> start =
	    std::chrono::system_clock::now();
	skymap->compute();
	std::chrono::time_point<std::chrono::system_clock> stop =
	    std::chrono::system_clock::now();

	auto milliseconds =
	    std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	unsigned long pxl_speed =
	    milliseconds.count() / skymap->getNpix() * getThreadsNumber();
	
    print_pixel_time(pxl_speed);

	EXPECT_LE(pxl_speed, 1000);  // ms
}

TEST(PerformanceTest, BremsstrahlungIntegrator) {
    int nside = 4;
	std::vector<PID> particletypes = {Electron, Positron};
	auto dragonModel = std::make_shared<cosmicrays::Dragon2D>(
	    cosmicrays::Dragon2D(particletypes));
	auto bremsstrahlung = std::make_shared<interactions::BremsstrahlungTsai74>(
	    interactions::BremsstrahlungTsai74());
	auto ringModel = std::make_shared<neutralgas::RingModel>(
	    neutralgas::RingModel(neutralgas::GasType::HI));
	// integrator
	auto in = std::make_shared<BremsstrahlungIntegrator>(
	    BremsstrahlungIntegrator(dragonModel, ringModel, bremsstrahlung));
	auto Egamma = 1_GeV;
	auto skymap = std::make_shared<GammaSkymap>(GammaSkymap(nside, Egamma));
	skymap->setIntegrator(in);
	
    in->setupCacheTable(10, 10, 2);

	std::chrono::time_point<std::chrono::system_clock> start =
	    std::chrono::system_clock::now();
	skymap->compute();
	std::chrono::time_point<std::chrono::system_clock> stop =
	    std::chrono::system_clock::now();

	auto milliseconds =
	    std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	unsigned long pxl_speed =
	    milliseconds.count() / skymap->getNpix() * getThreadsNumber();
	
    print_pixel_time(pxl_speed);

	EXPECT_LE(pxl_speed, 1000);  // ms
}
	
TEST(PerformanceTest, DarkMatterIntegrator) {
    int nside = 8;
    auto dmSpectrum = std::make_shared<darkmatter::PPPC4DMIDSpectrum>(
	    darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu,
	                                  darkmatter::Mass::m100GeV));
	QMass M_200 =
	    0.7 * 8e11 *
	    sun_mass;  // Battaglia et al., 2005, MNRAS, 364 (converted to M_200)
	double concentration = 18;  // Battaglia et al., 2005, MNRAS, 364
	double gamma_slope = 1.;    // NFW
	auto dmProfile = std::make_shared<darkmatter::NFWGProfile>(
	    gamma_slope, concentration, M_200);
	auto in = std::make_shared<DarkMatterIntegrator>(dmSpectrum, dmProfile);
	auto Egamma = 10_GeV;
	auto skymap = std::make_shared<GammaSkymap>(GammaSkymap(nside, Egamma));
	skymap->setIntegrator(in);
	
    std::chrono::time_point<std::chrono::system_clock> start =
	    std::chrono::system_clock::now();
	skymap->compute();
	std::chrono::time_point<std::chrono::system_clock> stop =
	    std::chrono::system_clock::now();

	auto milliseconds =
	    std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	unsigned long pxl_speed =
	    milliseconds.count() / skymap->getNpix() * getThreadsNumber();
	
    print_pixel_time(pxl_speed);

	EXPECT_LE(pxl_speed, 100);  // ms
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

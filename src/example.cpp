#include "hermes.h"

#include <chrono>
#include <iostream>
#include <memory>

namespace hermes {

void exampleIC() {
    // cosmic ray density models
    auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
    std::vector<PID> particletypes = {Electron, Positron};
    auto dragonModel = std::make_shared<Dragon2DCRDensity>(Dragon2DCRDensity(
                           getDataPath("CosmicRays/Gaggero17/run_2D.fits.gz"),
                           //	"/home/andy/Projects/category_science/hermes-data/tmp_data/run_B-C_D03,7_delta0,45_vA13.fits.gz",
                           particletypes));

    // interaction
    auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());

    /*
    CacheStorageKN CKN;
    auto cacheKN = std::make_unique<CacheStorageKN>(std::move(CKN));
    kleinnishina->setCacheStorage(std::move(cacheKN));
    */

    // photon field model
    auto photonField = std::make_shared<CMB>(CMB());

    // integrator
    auto intIC = std::make_shared<InverseComptonIntegrator>(
                     InverseComptonIntegrator(dragonModel, photonField, kleinnishina));

    CacheStorageIC CIC;
    auto cache = std::make_unique<CacheStorageIC>(std::move(CIC));
    intIC->setCacheStorage(std::move(cache));

    // skymap
    int nside = 32;
    auto mask = std::make_shared<RectangularWindow>(RectangularWindow(
                    QAngle(8_deg), QAngle(-8_deg), QAngle(-80_deg), QAngle(80_deg)));
    auto skymaps = std::make_shared<DiffFluxSkymap>(DiffFluxSkymap(nside, 100_MeV));
    //auto skymaps = std::make_shared<DiffFluxSkymapRange>(DiffFluxSkymapRange(nside, 100_MeV, 300_GeV, 10));
    //skymaps->setMask(mask);
    skymaps->setIntegrator(intIC);

    auto output = std::make_shared<FITSOutput>(FITSOutput("!example-ic.fits.gz"));

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    skymaps->compute();
    std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
    skymaps->save(output);

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    float pxl_speed = milliseconds.count()/skymaps->getNpix()*skymaps->getThreadsNumber();
    std::cerr << "speed: " << pxl_speed << " ms/pxl" << std::endl;
}

void examplePiZero() {
    // cosmic ray density models
    auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
    std::vector<PID> particletypes = {Proton};
    auto dragonModel = std::make_shared<Dragon2DCRDensity>(Dragon2DCRDensity(
                           //		getDataPath("CosmicRays/Gaggero17/run_2D.fits.gz"),
                           "/home/andy/Projects/category_science/hermes-data/tmp_data/run_B-C_D03,7_delta0,45_vA13.fits.gz",
                           particletypes));

    // interaction
    auto kamae = std::make_shared<Kamae06>(Kamae06());
    /* 
    CacheStorageKamae KamaeCache;
    auto cacheKamae = std::make_unique<CacheStorageKamae>(std::move(KamaeCache));
    kamae->setCacheStorage(std::move(cacheKamae));
    */
    // HI model
    auto ringModel = std::make_shared<RingModelDensity>(RingModelDensity());

    // integrator
    auto intPiZero = std::make_shared<PiZeroIntegrator>(
                         PiZeroIntegrator(dragonModel, ringModel, kamae));

    // skymap
    int nside = 16;
    auto mask = std::make_shared<RectangularWindow>(RectangularWindow(
                    QAngle(8_deg), QAngle(-8_deg), QAngle(-80_deg), QAngle(80_deg)));
    auto skymaps = std::make_shared<DiffFluxSkymap>(DiffFluxSkymap(nside, 100_MeV));
    //auto skymaps = std::make_shared<DiffFluxSkymapRange>(DiffFluxSkymapRange(nside, 100_MeV, 300_GeV, 5));
    //skymaps->setMask(mask);
    skymaps->setIntegrator(intPiZero);

    auto output = std::make_shared<FITSOutput>(FITSOutput("!example-pion.fits.gz"));

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    skymaps->compute();
    std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
    skymaps->save(output);
       
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    float pxl_speed = milliseconds.count()/skymaps->getNpix()*skymaps->getThreadsNumber();

    std::cerr << "pxl spd: " << pxl_speed << " ms/pxl" << std::endl;
}


void exampleGeneric() {

    // magnetic field models
    auto JF12 = std::make_shared<JF12Field>(JF12Field());
    //JF12->randomStriated(137);
    //JF12->randomTurbulent(1337);
    auto PT11 = std::make_shared<PT11Field>(PT11Field());
    auto WMAP07 = std::make_shared<WMAP07Field>(WMAP07Field());
    auto Sun08 = std::make_shared<Sun08Field>(Sun08Field());

    // cosmic ray density models
    auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
    auto WMAP07Model = std::make_shared<WMAP07CRDensity>(WMAP07CRDensity());
    auto Sun08Model = std::make_shared<Sun08CRDensity>(Sun08CRDensity());
    //auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity(
    //	getDataPath("CosmicRays/Gaggero17/run_2D.fits"), Electron, DragonFileType::_2D));

    // gas models
    auto gasCordes91 = std::make_shared<HII_Cordes91>(HII_Cordes91());
    auto gasYMW16 = std::make_shared<YMW16>(YMW16());
    auto ringModel = std::make_shared<RingModelDensity>(RingModelDensity());

    // integrator
    auto intGeneric = std::make_shared<GenericIntegrator>(
                          GenericIntegrator(JF12, simpleModel, gasYMW16, ringModel));

    // skymap
    int nside = 64;
    auto skymap = std::make_shared<GenericSkymap>(GenericSkymap(nside, 10_MHz));
    skymap->setIntegrator(intGeneric);

    auto output = std::make_shared<FITSOutput>(FITSOutput("!example-generic.fits.gz"));

    skymap->compute();
    skymap->save(output);
}

void playground() {
    //exampleGeneric();
    examplePiZero();
    //exampleIC();
}

} // namespace hermes

int main(void) {
    hermes::playground();
    return 0;
}


#include "hermes.h"

#include <iostream>
#include <memory>

namespace hermes {

void examplePiZero() {

	// cosmic ray density models
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	std::vector<PID> particletypes = {Proton};
	auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity(
				getDataPath("CosmicRays/Gaggero17/run_2D.fits.gz"),
				particletypes, DragonFileType::_2D)); 

	// interaction
	auto kamae = std::make_shared<Kamae06>(Kamae06());

	// HI model
	auto ringModel = std::make_shared<RingModelDensity>(RingModelDensity());
	
	// integrator
	auto intPiZero = std::make_shared<PiZeroIntegrator>(
		PiZeroIntegrator(dragonModel, ringModel, kamae));

	// skymap
	int nside = 32;
        auto mask = std::make_shared<RectangularWindow>(RectangularWindow(
                        QAngle(8_deg), QAngle(-8_deg), QAngle(-80_deg), QAngle(80_deg)));
	//auto skymaps = std::make_shared<DiffFluxSkymap>(DiffFluxSkymap(nside, 1_GeV));
	auto skymaps = std::make_shared<DiffFluxSkymapRange>(DiffFluxSkymapRange(nside, 100_MeV, 100_GeV, 10));
	skymaps->setMask(mask);
	skymaps->setIntegrator(intPiZero);

	auto output = std::make_shared<FITSOutput>(FITSOutput("!example-pion.fits.gz"));
	
	skymaps->compute();
	skymaps->save(output);
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
}

} // namespace hermes

int main(void){

	hermes::playground();

	return 0;
}


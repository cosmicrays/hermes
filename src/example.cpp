#include "hermes.h"

#include <iostream>
#include <memory>

namespace hermes {

void exampleRM() {
	// magnetic field models
	auto B = Vector3QMField(0_muG, 0_muG, 1_muG);
	auto ufield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto WMAP07 = std::make_shared<WMAP07Field>(WMAP07Field());
	auto Sun08 = std::make_shared<Sun08Field>(Sun08Field());
	//Sun08->randomTurbulent(1337);
	auto JF12 = std::make_shared<JF12Field>(JF12Field());
	JF12->randomStriated(137);
	JF12->randomTurbulent(1337);
	auto PT11 = std::make_shared<PT11Field>(PT11Field());

	// gas models
	auto gasCordes91 = std::make_shared<HII_Cordes91>(HII_Cordes91());
	auto gasYMW16 = std::make_shared<YMW16>(YMW16());
	
	// integrator
	auto intRM = std::make_shared<RMIntegrator>(RMIntegrator(Sun08, gasYMW16));

	// skymap
	int nside = 32;	
	auto skymap = std::make_shared<RMSkymap>(RMSkymap(nside));
	skymap->setIntegrator(intRM);
	skymap->compute();

	// save
	auto output = std::make_shared<FITSOutput>(FITSOutput("!example.fits.gz"));
	skymap->save(output);
}

void exampleSynchro() {

	// magnetic field models
	auto B = Vector3QMField(0_muG, 0_muG, 1_muG);
	auto ufield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto JF12 = std::make_shared<JF12Field>(JF12Field());
	JF12->randomStriated(137);
	JF12->randomTurbulent(1337);
	auto PT11 = std::make_shared<PT11Field>(PT11Field());
	auto WMAP07 = std::make_shared<WMAP07Field>(WMAP07Field());
	auto Sun08 = std::make_shared<Sun08Field>(Sun08Field());
	
	// cosmic ray density models
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto WMAP07Model = std::make_shared<WMAP07CRDensity>(WMAP07CRDensity());
	auto Sun08Model = std::make_shared<Sun08CRDensity>(Sun08CRDensity());
	auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity(
				getDataPath("DragonRuns/run_2D.fits.gz"),
				Electron, DragonFileType::_2D)); 
	
	// integrator
	auto intSynchro = std::make_shared<SynchroIntegrator>(SynchroIntegrator(JF12, dragonModel));

	// skymap
	int nside = 8;
        auto mask = std::make_shared<RectangularWindow>(RectangularWindow(
                        QAngle(45_deg), QAngle(10_deg), QAngle(40_deg), QAngle(340_deg)));
	auto skymaps = std::make_shared<RadioSkymapRange>(RadioSkymapRange(nside, 100_MHz, 100_GHz, 20));
	//auto skymap = std::make_shared<RadioSkymap>(RadioSkymap(nside, 408_MHz));
	skymaps->setIntegrator(intSynchro);

	auto output = std::make_shared<FITSOutput>(FITSOutput("!example-synchro.fits.gz"));
	
	//auto energy = std::next(dragonModel->begin());
	/*for(auto energy = dragonModel->begin(); energy != dragonModel->end(); ++energy) {
		Vector3QLength pos(8.19_kpc, 0, 0.097_kpc);
		auto density = dragonModel->getDensityPerEnergy(*energy, pos);
		std::cout << *energy << " " << density << std::endl;
	}*/
	
	skymaps->compute();
	skymaps->save(output);
}

void exampleFreeFree() {
	// gas models
	auto gasCordes91 = std::make_shared<HII_Cordes91>(HII_Cordes91());
	auto gasYMW16 = std::make_shared<YMW16>(YMW16());
	
	// integrator
	auto intFreeFree = std::make_shared<FreeFreeIntegrator>(FreeFreeIntegrator(gasYMW16));

	// skymap
	int nside = 16;	
	auto skymaps = std::make_shared<RadioSkymapRange>(RadioSkymapRange(nside, 10_MHz, 10_GHz, 10));
	skymaps->setIntegrator(intFreeFree);
	skymaps->compute();

	// save
	auto output = std::make_shared<FITSOutput>(FITSOutput("!example-freefree.fits.gz"));
	skymaps->save(output);
}


void exampleSynchroAbsorption() {

	// magnetic field models
	auto JF12 = std::make_shared<JF12Field>(JF12Field());
	JF12->randomStriated(137);
	JF12->randomTurbulent(1337);
	auto PT11 = std::make_shared<PT11Field>(PT11Field());
	auto WMAP07 = std::make_shared<WMAP07Field>(WMAP07Field());
	auto Sun08 = std::make_shared<Sun08Field>(Sun08Field());
	
	// cosmic ray density models
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto WMAP07Model = std::make_shared<WMAP07CRDensity>(WMAP07CRDensity());
	auto Sun08Model = std::make_shared<Sun08CRDensity>(Sun08CRDensity());
	//auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity(
	//	getDataPath("RingModelDensity/run_2D.fits"), Electron, DragonFileType::_2D)); 
	
	// gas models
	auto gasCordes91 = std::make_shared<HII_Cordes91>(HII_Cordes91());
	auto gasYMW16 = std::make_shared<YMW16>(YMW16());
	
	// integrator
	auto intSynchroAbsorption = std::make_shared<SynchroAbsorptionIntegrator>(
		SynchroAbsorptionIntegrator(JF12, simpleModel, gasYMW16));

	// skymap
	int nside = 16;
	auto skymaps = std::make_shared<RadioSkymapRange>(RadioSkymapRange(nside, 1_MHz, 10_GHz, 20));
	skymaps->setIntegrator(intSynchroAbsorption);

	auto output = std::make_shared<FITSOutput>(FITSOutput("!example-synchro-absorption.fits.gz"));
	
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
	//	getDataPath("RingModelDensity/run_2D.fits"), Electron, DragonFileType::_2D)); 
	
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
	//exampleRM();
	exampleSynchro();
	//exampleSynchroAbsorption();
	//exampleFreeFree();

}

} // namespace hermes

int main(void){

	hermes::playground();

	return 0;
}


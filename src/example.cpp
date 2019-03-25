#include "hermes/example.h"

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
	//auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity("/home/andy/Work/notebooks/Hermes/run_2D.fits", Electron, DragonFileType::_2D)); 
	
	// integrator
	auto intSynchro = std::make_shared<SynchroIntegrator>(SynchroIntegrator(JF12, simpleModel));
	//auto intFreeFree = std::make_shared<FreeFreeIntegrator>(FreeFreeIntegrator(ufield, simpleModel));

/*				
	std::cout << "# X\tY\tZ\teta" << std::endl;
	auto energy = std::next(dragonModel->begin());
	//energy++;
	QLength x = 1_kpc;
	QLength y = 1_kpc;
#pragma omp critical(print)
	for (QLength x = -12_kpc; x < 12_kpc; x += 0.5_kpc)
		for (QLength y = -12_kpc; y < 12_kpc; y += 0.5_kpc)
			for (QLength z = -4_kpc; z < 4_kpc; z += 0.2_kpc) {
				Vector3QLength pos(x.getValue(), y.getValue(), z.getValue());
				auto density = dragonModel->getDensityPerEnergy(*energy, pos);
				//auto density = simpleModel->getDensityPerEnergy(*energy, pos);
				//auto density = (JF12->getField(pos)).getZ();
				//auto density = (PT11->getField(pos)).getR();
				//auto density = (testField->getField(pos)).getR();
				//if (density.getValue() == 0) continue;
				std::cout << x.getValue()/1_pc << "\t" <<
					     y.getValue()/1_pc << "\t" <<
					     z.getValue()/1_pc << "\t" <<
					     density << std::endl;
			}
*/	

	// skymap
	int nside = 8;
	auto skymaps = std::make_shared<RadioSkymapRange>(RadioSkymapRange(nside, 100_MHz, 100_GHz, 10));
	//auto skymap = std::make_shared<RadioSkymap>(RadioSkymap(nside, 408_MHz));
	skymaps->setIntegrator(intSynchro);

	auto output = std::make_shared<FITSOutput>(FITSOutput("!example-multi.fits.gz"));
	
	skymaps->compute();
	//skymap->printPixels();
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
	//auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity("/home/andy/Work/notebooks/Hermes/run_2D.fits", Electron, DragonFileType::_2D)); 
	
	// gas models
	auto gasCordes91 = std::make_shared<HII_Cordes91>(HII_Cordes91());
	auto gasYMW16 = std::make_shared<YMW16>(YMW16());
	
	// integrator
	auto intSynchroAbsorption = std::make_shared<SynchroAbsorptionIntegrator>(
		SynchroAbsorptionIntegrator(JF12, simpleModel, gasYMW16));

	// skymap
	int nside = 8;
	auto skymaps = std::make_shared<RadioSkymapRange>(RadioSkymapRange(nside, 100_MHz, 100_GHz, 10));
	skymaps->setIntegrator(intSynchroAbsorption);

	auto output = std::make_shared<FITSOutput>(FITSOutput("!example-multi.fits.gz"));
	
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
	auto skymaps = std::make_shared<RadioSkymapRange>(RadioSkymapRange(nside, 100_MHz, 100_GHz, 10));
	skymaps->setIntegrator(intFreeFree);
	skymaps->compute();

	// save
	auto output = std::make_shared<FITSOutput>(FITSOutput("!example-multi.fits.gz"));
	skymaps->save(output);
}

void playground() {

	//exampleRM();
	//exampleSynchro();
	exampleSynchroAbsorption();
	//exampleFreeFree();
}

} // namespace hermes

int main(void){

	hermes::playground();

	return 0;
}


#include "hermes/example.h"

#include <iostream>
#include <memory>

namespace hermes {

void playground() {

	// magnetic field models
	auto B = Vector3QMField(1_muG, 0_muG, 0_muG);
	auto ptr_ufield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto ptr_JF12 = std::make_shared<JF12Field>(JF12Field());
	auto ptr_PT11 = std::make_shared<PT11Field>(PT11Field());

	// cosmic ray density models
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity(1_GeV, 1e4_GeV, 10));
	auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity("/home/andy/Work/notebooks/Hermes/run_3D.fits", Electron)); 

	// gas models
	auto ptr_Gas = std::make_shared<HII_Cordes91>(HII_Cordes91());

	// integrators
	auto synchro = SynchroIntegrator(ptr_JF12, dragonModel);
	auto RM = RMIntegrator(ptr_JF12, ptr_Gas);

	int nside = 16;	
	
	auto ptr_skymap = std::make_shared<SynchroSkymap>(SynchroSkymap(nside, 408_MHz));
	//auto ptr_skymap_range = std::make_shared<SynchroSkymapRange>(SynchroSkymap(nside, MinF, MaxF, nsteps));
	synchro.set_skymap(ptr_skymap);
	synchro.compute();
	
	//SynchroSkymapRange.save(FITSOutput("name.fits.gz"));
	

	//auto ptr_skymap = std::make_shared<RMSkymap>(RMSkymap(nside));
	//RM.set_skymap(ptr_skymap);
	//RM.compute();

#ifdef HERMES_HAVE_CFITSIO
	auto ptr_output = std::make_shared<FITSOutput>(FITSOutput("!example.fits.gz"));
	ptr_output->save(ptr_skymap);
#endif // HERMES_HAVE_CFITSIO

}

} // namespace hermes

int main(void){

	hermes::playground();

	return 0;
}


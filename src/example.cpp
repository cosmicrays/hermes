#include "hermes/example.h"

#include <iostream>
#include <memory>

#include <gsl/gsl_sf_synchrotron.h>

namespace hermes {

class TestGasDensity: public GasDensity {
public:
        TestGasDensity() { }
        QPDensity getDensity(const Vector3QLength &pos) const {
		return 1.0 / 1_cm3; 
	}
};

void playground() {

	// cosmic ray density model
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity(1_GeV, 1e4_GeV, 10));


	auto B = Vector3QMField(1_muG, 0_muG, 0_muG);
	auto ptr_ufield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto ptr_JF12 = std::make_shared<JF12Field>(JF12Field());

	auto pos = Vector3QLength(1_pc);
	std::cout << ptr_ufield->getField(pos).getPerpendicularTo(pos) << std::endl;

	auto synchro = SynchroIntegrator(ptr_JF12, simpleModel, 1_GHz);

	int nside = 16;	
	auto ptr_skymap = std::make_shared<SynchroSkymap>(SynchroSkymap(nside));
	synchro.set_skymap(ptr_skymap);
	synchro.compute();

	/*
	auto ptr_rmskymap = std::make_shared<RMSkymap>(RMSkymap(nside));
	auto ptr_Gas = std::make_shared<HII_Cordes91>(HII_Cordes91());
	//auto ptr_Gas = std::make_shared<TestGasDensity>(TestGasDensity());


	RMIntegrator RM = RMIntegrator(ptr_JF12, ptr_Gas);
	RM.set_skymap(ptr_rmskymap);

	RM.compute(); */

#ifdef HERMES_HAVE_CFITSIO
	auto ptr_output = std::make_shared<FITSOutput>(FITSOutput("!example.fits.gz"));
	ptr_output->save(
		ptr_skymap
	);
#endif // HERMES_HAVE_CFITSIO

}

} // namespace hermes

int main(void){

	hermes::playground();

	return 0;
}


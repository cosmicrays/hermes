#include "hermes/example.h"

#include <iostream>
#include <memory>

namespace hermes {

void playground() {

	QEnergy E;
	Vector3MField B(1.0_muG);
	Vector3Length vec_x(1.0_kpc);

	//UniformMagneticField UField = UniformMagneticField(B);
	
	auto ptr_skymap = std::make_shared<RMSkymap>(RMSkymap(16));
	std::shared_ptr<Skymap> ptr_skymap_2;

	ptr_skymap_2 = ptr_skymap;

	auto ptr_JF12 = std::make_shared<JF12Field>(JF12Field());
	auto ptr_Gas = std::make_shared<HII_Cordes91>(HII_Cordes91());
	auto ptr_output = std::make_shared<FITSOutput>(FITSOutput("!example.fits.gz"));

	RMIntegrator RM = RMIntegrator(ptr_JF12, ptr_Gas);
	RM.set_skymap(ptr_skymap);
	RM.compute();

	ptr_output->save(
		ptr_skymap
	);

	
}

} // namespace hermes

int main(void){

	hermes::playground();

	return 0;
}


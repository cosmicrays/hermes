#include "hermes/integrators/Integrator.h"
#include <iostream>

namespace hermes {

Integrator::Integrator() { }

Integrator::~Integrator() { }

void Integrator::set_skymap(std::shared_ptr<Skymap> skymap) {
	working_skymap = skymap;
}

void Integrator::compute() {
	unsigned long ipix;

	if(working_skymap == 0)
		// TODO: transform to exception
		std::cout << "Provide a skymap with the set_skymap() method" << std::endl;
	else {
		// TODO: OpenMP will go here
		for (Skymap::iterator pixel = working_skymap->begin(); pixel != working_skymap->end(); ++pixel) {
			ipix = pixel - working_skymap->begin();
			iterdir = pix2ang_ring(working_skymap->getNside(), ipix);
 			*pixel = this->integral(iterdir);
		}
	
	}
}

} // namespace hermes 

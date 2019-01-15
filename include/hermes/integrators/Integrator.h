#ifndef HERMES_INTEGRATOR_H
#define HERMES_INTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/Grid.h"
#include "hermes/HEALPix.h"
#include "hermes/Common.h"

#if _OPENMP
#include <omp.h>
#define OMP_SCHEDULE static,100
#endif

#include <memory>

namespace hermes {

template <class T>
class Integrator {
public:
	typedef typename T::iterator tPixelIterator;

	std::shared_ptr<T> working_skymap;
	QDirection iterdir;

	inline Integrator() { };

	inline void set_skymap(std::shared_ptr<T> skymap) {
		working_skymap = skymap;
	}

	/**
 		Every child class should implement this method which provides an expression
		of a given direction prepared for the integration
	*/
	virtual typename T::tPixel integrateOverLOS(QDirection iterdir) = 0;

	inline void compute() {
		unsigned long ipix;

#if _OPENMP
       		std::cout << "hermes::Integrator: Number of Threads: " << omp_get_max_threads() << std::endl;
#endif

		if(working_skymap == nullptr)
			// TODO: transform to exception
			std::cout << "Provide a skymap with the set_skymap() method" << std::endl;
		else {
#pragma omp parallel for schedule(OMP_SCHEDULE)
			for (ipix = 0; ipix < working_skymap->size(); ++ipix) {
				iterdir = pix2ang_ring(working_skymap->getNside(), ipix);
				working_skymap->updatePixel(ipix, integrateOverLOS(iterdir));
			}
	
		}
	}

};

} // namespace hermes

#endif // HERMES_INTEGRATOR_H

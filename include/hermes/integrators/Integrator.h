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

#include <array>
#include <memory>

namespace hermes {

template <class T>
class Integrator {
public:
	typedef std::array<QAngle, 2> tDirection;
	typedef typename T::iterator tPixelIterator;

	std::shared_ptr<T> working_skymap;
	tDirection iterdir;

	inline Integrator() { };

	inline void set_skymap(std::shared_ptr<T> skymap) {
		working_skymap = skymap;
	}

	virtual typename T::tPixel integral(tDirection iterdir) = 0;

	/**
		Gives the distance from the galactic centre (GC) by providing the distance 
		from the Sun and the direction (theta,phi)
	*/
	inline QLength distanceFromGC(tDirection direction, QLength distanceFromSun) {
		//	R2 = (R_sun > 0) ? pow(R_sun, 2) + pow(d * cos(b), 2) - 2.0 * R_sun * d * cos(b) * cos(l) 
                //        : pow(R_sun, 2) + pow(d * cos(b), 2) + 2.0 * R_sun * d * cos(b) * cos(l); 

		//       if (R2 < pc * pc) 
		// 	   R2 = pc * pc;
		// 	R = sqrt(R2);
		QAngle theta = direction[0];
		QAngle phi = direction[1];
		Vector3QLength Sun_pos(8.5_kpc, 0_kpc, 0_kpc);
		return sqrt(squared(Sun_pos.getX()) + squared(distanceFromSun * cos(theta)) \
				- 2.*distanceFromSun*Sun_pos.getX()*cos(theta)*cos(phi));
	}

	inline Vector3QLength offsetSunToGC(Vector3QLength v) {
		Vector3QLength Sun_pos(8.5_kpc, 0_kpc, 0_kpc);
		return v + Sun_pos; 
	}

	inline void compute() {
		unsigned long ipix;

#if _OPENMP
       		std::cout << "hermes::Integrator: Number of Threads: " << omp_get_max_threads() << std::endl;
#endif

		if(working_skymap == 0)
			// TODO: transform to exception
			std::cout << "Provide a skymap with the set_skymap() method" << std::endl;
		else {
#pragma omp parallel for schedule(OMP_SCHEDULE)
			for (ipix = 0; ipix < working_skymap->size(); ++ipix) {
				iterdir = pix2ang_ring(working_skymap->getNside(), ipix);
				working_skymap->updatePixel(ipix, integral(iterdir));
			}
	
		}
	}

};

} // namespace hermes

#endif // HERMES_INTEGRATOR_H

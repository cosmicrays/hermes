#ifndef HERMES_INTEGRATOR_H
#define HERMES_INTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/Grid.h"
#include "hermes/HEALPix.h"

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

	inline QLength distanceFromGC(tDirection direction, QLength distanceFromSun) {
		QAngle theta = direction[0];
		QAngle phi = direction[1];
		QLength R_sun = 8_kpc;
		return sqrt(squared(R_sun) + squared(distanceFromSun * cos(theta)) \
				- 2.*distanceFromSun*R_sun*cos(theta)*cos(phi));
	}
	
	inline Vector3Length sphericalToCartesian(QLength r, QAngle theta, QAngle phi) {

		Vector3Length v;
		v.x = r * sin(theta) * cos(phi);
		v.z = r * sin(theta) * sin(phi);
		v.y = r * cos(theta);
		return v;
		// 	x = (R_sun > 0) ? R_sun - d * cos(b) * cos(l) : R_sun + d * cos(b) * cos(l);
		//	y = d * cos(b) * sin(l);
		//	z = d * sin(b);
	}

	inline void compute() {
		unsigned long ipix;

		if(working_skymap == 0)
			// TODO: transform to exception
			std::cout << "Provide a skymap with the set_skymap() method" << std::endl;
		else {
			// TODO: OpenMP will go here
			for (tPixelIterator pixel = working_skymap->begin(); pixel != working_skymap->end(); ++pixel) {
				ipix = pixel - working_skymap->begin();
				iterdir = pix2ang_ring(working_skymap->getNside(), ipix);
				*pixel = integral(iterdir);
			}
	
		}
	}

};

} // namespace hermes

#endif // HERMES_INTEGRATOR_H

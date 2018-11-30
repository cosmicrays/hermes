#include "hermes/integrators/RMIntegrator.h"

#include <iostream>

namespace hermes {

RMIntegrator::RMIntegrator(const std::shared_ptr<MagneticField> mfield,
			   const std::shared_ptr<GasDensity> gdensity) : mfield(mfield), gdensity(gdensity) {	
}

RMIntegrator::~RMIntegrator() { }

RMSkymap::tPixel RMIntegrator::integral(QDirection direction) {
	constexpr auto const_a0 = pow<3>(e_plus)/(8*pi*pi*epsilon0*squared(m_electron)*pow<3>(c_light));
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	
	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction);

	// TODO: quick hack > implement generic coordinate system transformations
	direction[1] = pi*radian-direction[1]; // rotate for JF12 (tmp)

	QRotationMeasure sum(0);
	QLength delta_d = 10.0_pc;
	// TODO: implement sophisticated adaptive integration method :-)
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos.setRThetaPhi(dist, direction[0], direction[1]);
		pos += positionSun;

		sum += (const_a0 * mfield->getField(pos).getR() * gdensity->getDensity(pos) * delta_d) * radian;
	}
	return sum;
}

} // namespace hermes 

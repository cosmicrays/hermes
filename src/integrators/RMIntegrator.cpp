#include "hermes/integrators/RMIntegrator.h"

#include <iostream>

namespace hermes {

RMIntegrator::RMIntegrator(const std::shared_ptr<MagneticField> mfield,
			   const std::shared_ptr<ChargedGasDensity> gdensity) : mfield(mfield), gdensity(gdensity) {	
}

RMIntegrator::~RMIntegrator() { }

QRotationMeasure RMIntegrator::integrateOverLOS(QDirection direction) const {
	constexpr auto const_a0 = pow<3>(e_plus)/(8*pi*pi*epsilon0*squared(m_electron)*pow<3>(c_light));
	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	Vector3QMField B;
	QMField B_parallel;

	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction);

	QRotationMeasure sum(0);
	QLength delta_d = 5.0_pc;
	// TODO: implement sophisticated adaptive integration method :-)

	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, direction);

		B = mfield->getField(pos);
		if (B.getR() == 0_muG) continue;
		// TODO: optimise
		B_parallel = B.getR() * cos((B.getValue()).getAngleTo(pos.getValue()));

		sum += (const_a0 * B_parallel * gdensity->getDensity(pos) * delta_d) * radian;
	}
	return sum;
}

} // namespace hermes 

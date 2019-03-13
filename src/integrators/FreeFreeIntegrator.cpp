#include "hermes/integrators/FreeFreeIntegrator.h"

#include <iostream>
#include <memory>
#include <gsl/gsl_sf_synchrotron.h>

namespace hermes {

FreeFreeIntegrator::FreeFreeIntegrator(
	const std::shared_ptr<MagneticField> mfield_,
	const std::shared_ptr<CosmicRayDensity> crdensity_) : 
	mfield(mfield_), crdensity(crdensity_) {	
}

FreeFreeIntegrator::~FreeFreeIntegrator() { }

QTemperature FreeFreeIntegrator::intensityToTemperature(
		QIntensity intensity_, QFrequency freq_) const {
	return intensity_*c_squared /
		(2*freq_*freq_*k_boltzmann);
}

QTemperature FreeFreeIntegrator::integrateOverLOS(
		QDirection direction) const {
	return integrateOverLOS(direction, 408_MHz);
}

QTemperature FreeFreeIntegrator::integrateOverLOS(
		QDirection direction, QFrequency freq_) const {

	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	QIntensity total_intensity(0);
	QLength delta_d = 100.0_pc;

	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction);

	// TODO: implement sophisticated adaptive integration method :-)
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, direction);

		Vector3QMField B;
        	B = mfield->getField(pos);

#pragma omp critical(stdcout)
		std::cout << pos.getX()/1_pc << "\t" <<
			     pos.getY()/1_pc << "\t" <<
			     pos.getZ()/1_pc << "\t" <<
			     B.getR() << std::endl;
	}
	return intensityToTemperature(total_intensity / 4_pi, freq_);
}

} // namespace hermes 

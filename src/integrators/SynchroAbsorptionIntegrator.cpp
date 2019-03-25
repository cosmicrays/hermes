#include "hermes/integrators/SynchroAbsorptionIntegrator.h"
#include "hermes/Common.h"

#include <memory>
#include <gsl/gsl_sf_synchrotron.h>

namespace hermes {

SynchroAbsorptionIntegrator::SynchroAbsorptionIntegrator(
	const std::shared_ptr<MagneticField> mfield_,
	const std::shared_ptr<CosmicRayDensity> crdensity_,
	const std::shared_ptr<GasDensity> gdensity_) : 
	SynchroIntegrator(mfield_, crdensity_),
	FreeFreeIntegrator(gdensity_) {
}

SynchroAbsorptionIntegrator::~SynchroAbsorptionIntegrator() { }

QTemperature SynchroAbsorptionIntegrator::integrateOverLOS(
		QDirection direction) const {
	return integrateOverLOS(direction, 408_MHz);
}

QTemperature SynchroAbsorptionIntegrator::integrateOverLOS(
		QDirection direction, QFrequency freq_) const {

	Vector3QLength positionSun(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	QIntensity total_intensity(0);
	QLength delta_d = 10.0_pc;

	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(positionSun, direction);

	// TODO: implement sophisticated adaptive integration method :-)
	for(QLength dist = 0; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(positionSun, dist, direction);

		//if (crdensity->existsScaleFactor())
			total_intensity +=
				integrateOverLogEnergy(pos, freq_) * delta_d;
		//else
		//	total_intensity +=
		//		integrateOverEnergy(pos, freq_) * delta_d;
	}
	return intensityToTemperature(total_intensity / 4_pi, freq_);
}

} // namespace hermes 

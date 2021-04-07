#include "hermes/integrators/SynchroAbsorptionIntegrator.h"

#include <memory>

#include "hermes/Common.h"

namespace hermes {

SynchroAbsorptionIntegrator::SynchroAbsorptionIntegrator(
    const std::shared_ptr<magneticfields::MagneticField>& mfield_,
    const std::shared_ptr<cosmicrays::CosmicRayDensity>& crdensity_,
    const std::shared_ptr<ionizedgas::IonizedGasDensity>& gdensity_)
    : RadioIntegratorTemplate("SynchroAbsorption"), mfield(mfield_), crdensity(crdensity_), gdensity(gdensity_) {
	intSynchro = std::make_shared<SynchroIntegrator>(SynchroIntegrator(mfield, crdensity));
	intFreeFree = std::make_shared<FreeFreeIntegrator>(FreeFreeIntegrator(gdensity));
}

SynchroAbsorptionIntegrator::~SynchroAbsorptionIntegrator() {}

QTemperature SynchroAbsorptionIntegrator::integrateOverLOS(const QDirection& direction) const {
	return integrateOverLOS(direction, 408_MHz);
}

QTemperature SynchroAbsorptionIntegrator::integrateOverLOS(const QDirection& direction_,
                                                           const QFrequency& freq_) const {
	Vector3QLength observerPosition(8.5_kpc, 0, 0);
	Vector3QLength pos(0.0);
	QIntensity total_intensity(0);
	QLength delta_d = 10.0_pc;

	QNumber opticalDepth(0);
	std::vector<QNumber> opticalDepthLOS;

	// distance from the (spherical) galactic border in the given direction
	QLength maxDistance = distanceToGalBorder(observerPosition, direction_);

	for (QLength dist = delta_d; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(observerPosition, dist, direction_);
		opticalDepth += intFreeFree->absorptionCoefficient(pos, freq_) * delta_d;
		opticalDepthLOS.push_back(opticalDepth);
	}

	// TODO(adundovi): implement sophisticated adaptive integration method :-)
	auto opticalDepthIter = opticalDepthLOS.begin();
	for (QLength dist = delta_d; dist <= maxDistance; dist += delta_d) {
		pos = getGalacticPosition(observerPosition, dist, direction_);
		total_intensity += intSynchro->integrateOverEnergy(pos, freq_) / 4_pi *
		                   exp((*opticalDepthIter) - opticalDepthLOS[opticalDepthLOS.size() - 1]) * delta_d;
		++opticalDepthIter;
	}

	return intensityToTemperature(total_intensity, freq_);
}

}  // namespace hermes

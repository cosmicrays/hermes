#include "hermes/integrators/BremsstrahlungIntegrator.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <mutex>
#include <numeric>
#include <thread>

#include "hermes/Common.h"
#include "hermes/integrators/LOSIntegrationMethods.h"

namespace hermes {

BremsstrahlungIntegrator::BremsstrahlungIntegrator(
    const std::shared_ptr<cosmicrays::CosmicRayDensity> &crDensity_,
    const std::shared_ptr<neutralgas::RingModel> &ngdensity_,
    const std::shared_ptr<interactions::BremsstrahlungAbstract> &crossSec_)
    : PiZeroIntegrator(crDensity_, ngdensity_, crossSec_),
      crossSecBrem(crossSec_) {}

BremsstrahlungIntegrator::~BremsstrahlungIntegrator() {}

QPiZeroIntegral BremsstrahlungIntegrator::integrateOverEnergy(
    const Vector3QLength &pos_, const QEnergy &Egamma_) const {
	if (cacheTableInitialized) return getIOEfromCache(pos_, Egamma_);

	auto crDensity = crList[0];

	// we have 3 targets: HI, HII and He
	// the ring model has HI and H2 (CO)

	// Not ionized

	// brems = cs_HI + cs_He*He_abundance
	std::vector<QDiffCrossSection> diffCrossSectionVector;
	std::transform(
	    crDensity->beginAfterEnergy(Egamma_), crDensity->end(),
	    std::back_inserter(diffCrossSectionVector),
	    [this, Egamma_](const QEnergy &E) -> QDiffCrossSection {
		    return (crossSecBrem->getDiffCrossSectionForTarget(
		                interactions::BremsstrahlungAbstract::Target::HI, E,
		                Egamma_) +
		            0.1 * crossSecBrem->getDiffCrossSectionForTarget(
		                      interactions::BremsstrahlungAbstract::Target::He,
		                      E, Egamma_));
	    });

	auto pid_projectile = crDensity->getPID();

	std::vector<QPDensity> cosmicRayVector;
	std::transform(crDensity->beginAfterEnergy(Egamma_), crDensity->end(),
	               std::back_inserter(cosmicRayVector),
	               [crDensity, pos_](const QEnergy &E) -> QPDensity {
		               return crDensity->getDensityPerEnergy(E, pos_) * E;
	               });

	std::vector<QPiZeroIntegral> integral;
	std::transform(cosmicRayVector.begin(), cosmicRayVector.end(),
	               diffCrossSectionVector.begin(), std::back_inserter(integral),
	               [](const QPDensity &a, const QDiffCrossSection &b) {
		               return a * b * c_light;
	               });

	// log-integration
	auto integralOverEnergy =
	    std::log(crDensity->getEnergyScaleFactor()) *
	    std::accumulate(integral.begin(), integral.end(), QPiZeroIntegral(0));

	return integralOverEnergy;
}

}  // namespace hermes

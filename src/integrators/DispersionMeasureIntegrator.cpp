#include "hermes/integrators/DispersionMeasureIntegrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"

namespace hermes {

DispersionMeasureIntegrator::DispersionMeasureIntegrator(
    const std::shared_ptr<chargedgas::ChargedGasDensity> gdensity)
    : DispersionMeasureIntegratorTemplate(), gdensity(gdensity) {}

DispersionMeasureIntegrator::~DispersionMeasureIntegrator() {}

QDispersionMeasure
DispersionMeasureIntegrator::integrateOverLOS(QDirection direction) const {

	auto integrand = [this, direction](const QLength &dist) {
		return gdensity->getDensity(
		    getGalacticPosition(getSunPosition(), dist, direction));
	};

	return gslQAGIntegration<QDispersionMeasure, QPDensity>(
	    [this, integrand](QLength dist) { return integrand(dist); }, 0,
	    getMaxDistance(direction), 500);
}

DispersionMeasureIntegrator::tLOSProfile
DispersionMeasureIntegrator::getLOSProfile(QDirection direction,
					   int Nsteps) const {

	auto integrand = [this, direction](const QLength &dist) {
		return gdensity->getDensity(
		    getGalacticPosition(getSunPosition(), dist, direction));
	};

	QLength start = 0_m;
	QLength stop = getMaxDistance(direction);
	QLength delta_d = (stop - start) / Nsteps;

	tLOSProfile profile;

	for (QLength dist = start; dist <= stop; dist += delta_d) {
		profile.first.push_back(dist);
		profile.second.push_back(static_cast<double>(integrand(dist)));
	}

	return profile;
}

} // namespace hermes

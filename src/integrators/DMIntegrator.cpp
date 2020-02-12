#include "hermes/integrators/DMIntegrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"

namespace hermes {

DMIntegrator::DMIntegrator(const std::shared_ptr<ChargedGasDensity> gdensity) :
	DMIntegratorTemplate(),
	gdensity(gdensity) {	

}

DMIntegrator::~DMIntegrator() { }

QDispersionMeasure DMIntegrator::integrateOverLOS(QDirection direction) const {

	auto integrand = [this, direction](const QLength &dist) {
		return (this->gdensity)->getDensity(getGalacticPosition(this->getSunPosition(), dist, direction)); };
	
	return gslQAGIntegration<QDispersionMeasure, QPDensity>(
			[this, integrand](QLength dist) {return integrand(dist);}, 0, getMaxDistance(direction), 500);

}

DMIntegrator::tLOSProfile DMIntegrator::getLOSProfile(QDirection direction, int Nsteps) const {

	auto integrand = [this, direction](const QLength &dist) {
		return (this->gdensity)->getDensity(getGalacticPosition(this->getSunPosition(), dist, direction)); };

	QLength start = 0_m;
	QLength stop  = getMaxDistance(direction);
	QLength delta_d = (stop-start)/Nsteps;

	tLOSProfile profile;

	for(QLength dist = start; dist <= stop; dist += delta_d) {
		profile.first.push_back(dist);
		profile.second.push_back(static_cast<double>(integrand(dist)));
	}

	return profile;
}


} // namespace hermes 

#include "hermes/integrators/DMIntegrator.h"
#include "hermes/integrators/LOSIntegrationMethods.h"

namespace hermes {

DMIntegrator::DMIntegrator(const std::shared_ptr<ChargedGasDensity> gdensity) : gdensity(gdensity) {	
}

DMIntegrator::~DMIntegrator() { }

QDispersionMeasure DMIntegrator::integrateOverLOS(QDirection direction) const {
	
	auto integrand = [this, direction](const QLength &dist) {
		return (this->gdensity)->getDensity(getGalacticPosition(this->positionSun, dist, direction)); };

	return simpsonIntegration<QDispersionMeasure, QPDensity>(
			[integrand](QLength dist) {return integrand(dist);}, 0, getMaxDistance(direction), 500);
}

} // namespace hermes 

#ifndef HERMES_DMINTEGRATOR_H
#define HERMES_DMINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/skymaps/DMSkymap.h"
#include "hermes/integrators/Integrator.h"
#include "hermes/chargedGasDensity/ChargedGasDensity.h"

#include <memory>

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

typedef IntegratorTemplate<QDispersionMeasure, QNumber> DMIntegratorTemplate;

class DMIntegrator: public DMIntegratorTemplate {
private:
	std::shared_ptr<ChargedGasDensity> gdensity;
public:
	typedef std::pair<std::vector<QLength>, std::vector<double>> tLOSProfile;
	
	DMIntegrator(const std::shared_ptr<ChargedGasDensity> gdensity);
	~DMIntegrator();
	
	QDispersionMeasure integrateOverLOS(QDirection iterdir) const;
	
	/**
		Get the line of sight profile (integrand of integrateOverLOS) of a direction
		where N is equidistant number of steps from the Sun's position to the
		galactic border
	*/
	tLOSProfile getLOSProfile(QDirection direction, int Nsteps) const;
};

/** @}*/
} // namespace hermes

#endif // HERMES_DMINTEGRATOR_H

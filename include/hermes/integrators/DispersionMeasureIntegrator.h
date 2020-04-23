#ifndef HERMES_DISPERSIONMEASUREINTEGRATOR_H
#define HERMES_DISPERSIONMEASUREINTEGRATOR_H

#include <memory>

#include "hermes/Units.h"
#include "hermes/chargedgas/ChargedGasDensity.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/skymaps/DispersionMeasureSkymap.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

typedef IntegratorTemplate<QDispersionMeasure, QNumber>
    DispersionMeasureIntegratorTemplate;

class DispersionMeasureIntegrator : public DispersionMeasureIntegratorTemplate {
  private:
	std::shared_ptr<chargedgas::ChargedGasDensity> gdensity;

  public:
	typedef std::pair<std::vector<QLength>, std::vector<double>> tLOSProfile;

	DispersionMeasureIntegrator(
	    const std::shared_ptr<chargedgas::ChargedGasDensity> &gdensity);
	~DispersionMeasureIntegrator();

	QDispersionMeasure integrateOverLOS(const QDirection &iterdir) const;

	/**
	    Get the line of sight profile (integrand of integrateOverLOS) of
	   a direction where N is equidistant number of steps from the Sun's
	   position to the galactic border
	*/
	tLOSProfile getLOSProfile(const QDirection &direction, int Nsteps) const;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_DISPERSIONMEASUREINTEGRATOR_H

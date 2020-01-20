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

class DMIntegrator: public IntegratorTemplate<QDispersionMeasure, QNumber> {
private:
	std::shared_ptr<ChargedGasDensity> gdensity;
public:
	DMIntegrator(const std::shared_ptr<ChargedGasDensity> gdensity);
	~DMIntegrator();
	QDispersionMeasure integrateOverLOS(QDirection iterdir) const;
};

/** @}*/
} // namespace hermes

#endif // HERMES_DMINTEGRATOR_H

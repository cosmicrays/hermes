#ifndef HERMES_RMINTEGRATOR_H
#define HERMES_RMINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/skymaps/RMSkymap.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/magneticField/MagneticField.h"
#include "hermes/chargedGasDensity/ChargedGasDensity.h"

#include <memory>
#include <array>

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

typedef IntegratorTemplate<QRotationMeasure, QNumber> RMIntegratorTemplate;

class RMIntegrator: public IntegratorTemplate<QRotationMeasure, QNumber> {
private:
	std::shared_ptr<MagneticField> mfield;
	std::shared_ptr<ChargedGasDensity> gdensity;

	QRMIntegral integralFunction(Vector3QLength pos) const;
public:
	RMIntegrator(const std::shared_ptr<MagneticField> mfield,
		     const std::shared_ptr<ChargedGasDensity> gdensity);
	~RMIntegrator();
	QRotationMeasure integrateOverLOS(QDirection iterdir) const;
};

/** @}*/
} // namespace hermes

#endif // HERMES_RMINTEGRATOR_H

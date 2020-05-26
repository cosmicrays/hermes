#ifndef HERMES_RMINTEGRATOR_H
#define HERMES_RMINTEGRATOR_H

#include <array>
#include <memory>

#include "hermes/Units.h"
#include "hermes/chargedgas/ChargedGasDensity.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/magneticfields/MagneticField.h"
#include "hermes/skymaps/RotationMeasureSkymap.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

typedef IntegratorTemplate<QRotationMeasure, QNumber>
    RotationMeasureIntegratorTemplate;

class RotationMeasureIntegrator
    : public IntegratorTemplate<QRotationMeasure, QNumber> {
  private:
	std::shared_ptr<magneticfields::MagneticField> mfield;
	std::shared_ptr<chargedgas::ChargedGasDensity> gdensity;

	QRMIntegral integralFunction(const Vector3QLength& pos) const;

  public:
	RotationMeasureIntegrator(
	    const std::shared_ptr<magneticfields::MagneticField>& mfield,
	    const std::shared_ptr<chargedgas::ChargedGasDensity>& gdensity);
	~RotationMeasureIntegrator();

	QRotationMeasure integrateOverLOS(const QDirection& iterdir) const override;
	QRotationMeasure integrateOverLOS(const QDirection& iterdir,
	                                  const QNumber& num) const override {
		return QRotationMeasure(0);
	}
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_RMINTEGRATOR_H

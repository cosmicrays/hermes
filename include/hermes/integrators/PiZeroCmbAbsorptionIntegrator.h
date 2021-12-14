#ifndef HERMES_PIZEROCMBABSORPTIONINTEGRATOR_H
#define HERMES_PIZEROCMBABSORPTIONINTEGRATOR_H

#include "hermes/integrators/PiZeroIntegrator.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class PiZeroCmbAbsorptionIntegrator : public PiZeroIntegrator {
  public:
	QInverseLength absorptionCoefficient(const QEnergy &Egamma) const;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_PIZEROCMBABSORPTIONINTEGRATOR_H

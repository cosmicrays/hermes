#ifndef HERMES_NEUTRALGAS_PROFILEABSTRACT_H
#define HERMES_NEUTRALGAS_PROFILEABSTRACT_H

#include "hermes/Units.h"
#include "hermes/Vector3Quantity.h"
#include "hermes/neutralgas/GasType.h"

namespace hermes { namespace neutralgas {
/**
 * \addtogroup NeutralGas
 * @{
 */

using namespace units;

class ProfileAbstract {
  public:
	ProfileAbstract() {}
	virtual ~ProfileAbstract() {}

	virtual QPDensity getPDensity(GasType gas,
	                              const Vector3QLength &pos) const = 0;
};

/** @}*/
}}  // namespace hermes::neutralgas

#endif  // HERMES_NEUTRALGAS_PROFILEABSTRACT_H

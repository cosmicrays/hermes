#ifndef HERMES_NEUTRALGAS_NAKANISHI06_H
#define HERMES_NEUTRALGAS_NAKANISHI06_H

#include "hermes/neutralgas/ProfileAbstract.h"

namespace hermes { namespace neutralgas {
/**
 * \addtogroup NeutralGas
 * @{
 */

class Nakanishi06 : public ProfileAbstract {
  private:
  public:
	Nakanishi06();
	~Nakanishi06();

	QPDensity getHIDensity(const Vector3QLength &pos) const;
	QPDensity getH2Density(const Vector3QLength &pos) const;
	QPDensity getPDensity(GasType gas,
	                      const Vector3QLength &pos) const override;
};

/** @}*/
}}  // namespace hermes::neutralgas

#endif  // HERMES_NEUTRALGAS_NAKANISHI06_H

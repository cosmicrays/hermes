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

	QPDensity getPDensity(const Vector3QLength &pos) const override;
};

/** @}*/
}}  // namespace hermes::neutralgas

#endif  // HERMES_NEUTRALGAS_NAKANISHI06_H

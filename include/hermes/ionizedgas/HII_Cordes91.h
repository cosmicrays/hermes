#ifndef HERMES_HIICORDES91_H
#define HERMES_HIICORDES91_H

#include "hermes/ionizedgas/IonizedGasDensity.h"

namespace hermes { namespace ionizedgas {
/**
 * \addtogroup IonizedGas
 * @{
 */

class HII_Cordes91 : public IonizedGasDensity {
  private:
	QPDensity fne1, fne2;
	QLength H1, A1, H2, A2, R2;

  public:
	HII_Cordes91();
	QPDensity getDensity(const Vector3QLength &pos) const override;
};

/** @}*/
}}  // namespace hermes::ionizedgas

#endif  // HERMES_HIICORDES91_H

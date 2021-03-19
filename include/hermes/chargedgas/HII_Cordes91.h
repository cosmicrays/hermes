#ifndef HERMES_HIICORDES91_H
#define HERMES_HIICORDES91_H

#include "hermes/chargedgas/ChargedGasDensity.h"

namespace hermes { namespace chargedgas {
/**
 * \addtogroup ChargedGas
 * @{
 */

/**
 * \class HII_Cordes91
 * \brief A model of free electron distribution based on Cordes et al. 1991.
 *
 * Cordes, J. M. et al. 1991, doi:10.1038/354121a0
 */
class HII_Cordes91 : public ChargedGasDensity {
  private:
	QPDensity fne1, fne2;
	QLength H1, A1, H2, A2, R2;

  public:
	HII_Cordes91();
	QPDensity getDensity(const Vector3QLength &pos) const override;
};

/** @}*/
}}  // namespace hermes::chargedgas

#endif  // HERMES_HIICORDES91_H

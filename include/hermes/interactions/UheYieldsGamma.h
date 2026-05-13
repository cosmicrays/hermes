#ifndef HERMES_UHEYIELDSGAMMA_H
#define HERMES_UHEYIELDSGAMMA_H

#include "hermes/interactions/UheYields.h"

namespace hermes { namespace interactions {
/**
 * \addtogroup Interactions
 * @{
 */

class UheYieldsGamma final : public UheYields {
  protected:
	double getXDNdX(double x) const override;

  public:
	UheYieldsGamma(UheHadronicModel model = UheHadronicModel::Sibyll23e);
};

/** @}*/
}}  // namespace hermes::interactions

#endif  // HERMES_UHEYIELDSGAMMA_H

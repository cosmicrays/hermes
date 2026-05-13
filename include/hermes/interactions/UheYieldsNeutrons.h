#ifndef HERMES_UHEYIELDSNEUTRONS_H
#define HERMES_UHEYIELDSNEUTRONS_H

#include "hermes/interactions/UheYields.h"

namespace hermes { namespace interactions {
/**
 * \addtogroup Interactions
 * @{
 */

class UheYieldsNeutrons final : public UheYields {
  protected:
	double getXDNdX(double x) const override;

  public:
	UheYieldsNeutrons(UheHadronicModel model = UheHadronicModel::Sibyll23e);
};

/** @}*/
}}  // namespace hermes::interactions

#endif  // HERMES_UHEYIELDSNEUTRONS_H

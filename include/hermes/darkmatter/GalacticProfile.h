#ifndef HERMES_DARKMATTER_GALACTICPROFILE_H
#define HERMES_DARKMATTER_GALACTICPROFILE_H

#include "hermes/Units.h"

namespace hermes { namespace darkmatter {
/**
 * \addtogroup DarkMatter
 * @{
 */

using namespace units;

class GalacticProfile {
  public:
	GalacticProfile() {}
	virtual ~GalacticProfile() {}
	virtual QMassDensity getMassDensity(QLength r) const = 0;
};

/** @}*/
}}  // namespace hermes::darkmatter

#endif  // HERMES_DARKMATTER_GALACTICPROFILE_H

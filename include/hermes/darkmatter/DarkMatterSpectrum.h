#ifndef HERMES_DARKMATTERSPECTRUM_H
#define HERMES_DARKMATTERSPECTRUM_H

#include "hermes/Units.h"

namespace hermes { namespace darkmatter {
/**
 * \addtogroup DarkMatter
 * @{
 */

using namespace units;

class DarkMatterSpectrum {
  public:
	virtual QEnergy getRestMassEnergy() const = 0;
	virtual QInverseEnergy getParticlesPerEnergy(QEnergy Egamma) const = 0;
};

/** @}*/
}}  // namespace hermes::darkmatter

#endif  // HERMES_DARKMATTERSPECTRUM_H

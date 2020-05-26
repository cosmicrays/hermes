#ifndef HERMES_BREITWHEELER_H
#define HERMES_BREITWHEELER_H

#include <memory>

#include "hermes/interactions/DiffCrossSection.h"

namespace hermes { namespace interactions {
/**
 * \addtogroup Interactions
 * @{
 */

class BreitWheeler {
  public:
	BreitWheeler();

	QArea getCrossSection(const QEnergy &Egamma, const QEnergy &Eph,
	                      const QAngle &theta) const;
	QArea integratedOverTheta(const QEnergy &Egamma, const QEnergy &Eph) const;
};

/** @}*/
}}  // namespace hermes::interactions

#endif  // HERMES_BREITWHEELER_H

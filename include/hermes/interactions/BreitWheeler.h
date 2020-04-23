#ifndef HERMES_BREITWHEELER_H
#define HERMES_BREITWHEELER_H

#include "hermes/interactions/DiffCrossSection.h"

#include <memory>

namespace hermes {
namespace interactions {

class BreitWheeler {
  public:
    BreitWheeler();

    QArea getCrossSection(const QEnergy &Egamma, const QEnergy &Eph,
			  const QAngle &theta) const;
	double integrateOverTheta(const QEnergy &Egamma, const QEnergy &Eph) const;
};

} // namespace interactions
} // namespace hermes

#endif // HERMES_BREITWHEELER_H

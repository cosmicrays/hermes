#ifndef HERMES_SIMPLECRDENSITY_H
#define HERMES_SIMPLECRDENSITY_H

#include "hermes/cosmicrays/CosmicRayDensity.h"

namespace hermes {
namespace cosmicrays {

class SimpleCRDensity : public CosmicRayDensity {
  private:
    QEnergy minE, maxE;
    int steps;
    void makeEnergyRange();

  public:
    SimpleCRDensity(const PID &pid = Proton);
    SimpleCRDensity(const PID &pid, QEnergy minE, QEnergy maxE, int steps);
    QPDensityPerEnergy
    getDensityPerEnergy(const QEnergy &E_,
			const Vector3QLength &pos_) const override;
};

} // namespace cosmicrays
} // namespace hermes

#endif // HERMES_SIMPLECRDENSITY_H

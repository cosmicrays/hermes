#ifndef HERMES_DUMMYCRDENSITY_H
#define HERMES_DUMMYCRDENSITY_H

#include "hermes/cosmicrays/CosmicRayDensity.h"

namespace hermes { namespace cosmicrays {
/**
 * \addtogroup CosmicRays
 * @{
 */

class DummyCR : public CosmicRayDensity {
  private:
	QEnergy minE, maxE;
	int steps;
	void makeEnergyRange();

  public:
	DummyCR();
	DummyCR(const PID &pid);
	DummyCR(const PID &pid, const QEnergy &minE, const QEnergy &maxE,
	               int steps);
	QPDensityPerEnergy getDensityPerEnergy(
	    const QEnergy &E_, const Vector3QLength &pos_) const override;
};

/** @}*/
}}  // namespace hermes::cosmicrays

#endif  // HERMES_DUMMYCRDENSITY_H

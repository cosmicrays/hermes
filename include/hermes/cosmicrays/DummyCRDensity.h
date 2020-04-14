#ifndef HERMES_DUMMYCRDENSITY_H
#define HERMES_DUMMYCRDENSITY_H

#include "hermes/cosmicrays/CosmicRayDensity.h"

namespace hermes { namespace cosmicrays {

class DummyCRDensity: public CosmicRayDensity {
private:
	QEnergy minE, maxE;
	int steps;
	void makeEnergyRange();
public:
	DummyCRDensity();
	DummyCRDensity(const PID &pid);
	DummyCRDensity(const PID &pid, const QEnergy &minE, const QEnergy &maxE, int steps);
	QPDensityPerEnergy getDensityPerEnergy(const QEnergy& E_,
			const Vector3QLength& pos_) const override;
};

} // namespace cosmicrays
} // namespace hermes

#endif // HERMES_DUMMYCRDENSITY_H

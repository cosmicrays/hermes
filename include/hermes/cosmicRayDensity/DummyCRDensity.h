#ifndef HERMES_DUMMYCRDENSITY_H
#define HERMES_DUMMYCRDENSITY_H

#include "hermes/cosmicRayDensity/CosmicRayDensity.h"

namespace hermes {

class DummyCRDensity: public CosmicRayDensity {
private:
	QEnergy minE, maxE;
	int steps;
	void makeEnergyRange();
public:
	DummyCRDensity();
	DummyCRDensity(QEnergy minE, QEnergy maxE, int steps);
	QPDensityPerEnergy getDensityPerEnergy(const QEnergy& E_,
			const Vector3QLength& pos_) const override;
};

} // namespace hermes

#endif // HERMES_DUMMYCRDENSITY_H

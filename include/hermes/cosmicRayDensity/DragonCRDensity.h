#ifndef HERMES_DRAGONCRDENSITY_H
#define HERMES_DRAGONCRDENSITY_H

#include "hermes/cosmicRayDensity/CosmicRayDensity.h"

namespace hermes {

class DragonCRDensity: public CosmicRayDensity {
private:
	QEnergy minE, maxE;
	int steps;
	void makeEnergyRange();
public:
	DragonCRDensity();
	DragonCRDensity(QEnergy minE, QEnergy maxE, int steps);
	QPDensityPerEnergy getDensityPerEnergy(const Vector3QLength& pos, const QEnergy& E) const;

	iterator begin();
        const_iterator begin() const;
	iterator end();
	const_iterator end() const;
};

} // namespace hermes

#endif // HERMES_DRAGONCRDENSITY_H

#ifndef HERMES_SIMPLECRDENSITY_H
#define HERMES_SIMPLECRDENSITY_H

#include "hermes/cosmicRayDensity/CosmicRayDensity.h"

namespace hermes {

class SimpleCRDensity: public CosmicRayDensity {
private:
	QEnergy minE, maxE;
	int steps;
	void makeEnergyRange();
public:
	SimpleCRDensity();
	SimpleCRDensity(QEnergy minE, QEnergy maxE, int steps);
	QPDensityPerEnergy getDensity(const Vector3QLength& pos, const QEnergy& E) const;

	iterator begin();
        const_iterator begin() const;
	iterator end();
	const_iterator end() const;
};

} // namespace hermes

#endif // HERMES_SIMPLECRDENSITY_H

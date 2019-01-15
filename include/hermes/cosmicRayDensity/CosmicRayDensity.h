#ifndef HERMES_COSMICRAYDENSITY_H
#define HERMES_COSMICRAYDENSITY_H

#include "hermes/Units.h"
#include "hermes/Grid.h"

namespace hermes {

class CosmicRayDensity {
protected:
	typedef std::vector<QEnergy> tEnergyRange;
	mutable tEnergyRange energyRange;
public:
        virtual ~CosmicRayDensity() { }
        virtual QPDensityPerEnergy getDensityPerEnergy(
			const Vector3QLength& pos, const QEnergy& E) const = 0;
	
        typedef tEnergyRange::iterator iterator;
        typedef tEnergyRange::const_iterator const_iterator;
        iterator begin() {
		return energyRange.begin();	
	}
        const_iterator begin() const {
		return energyRange.begin();
	}
        iterator end() {
		return energyRange.end();
	}
        const_iterator end() const {
		return energyRange.end();
	}
};

} // namespace hermes

#endif // HERMES_COSMICRAYDENSITY_H

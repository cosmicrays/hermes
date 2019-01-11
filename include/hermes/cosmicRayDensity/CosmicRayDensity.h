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
        virtual QPDensityPerEnergy getDensity(
			const Vector3QLength& pos, const QEnergy& E) const = 0;
	
        typedef tEnergyRange::iterator iterator;
        typedef tEnergyRange::const_iterator const_iterator;
        virtual iterator begin() = 0;
        virtual const_iterator begin() const = 0;
        virtual iterator end() = 0;
        virtual const_iterator end() const = 0;
};

} // namespace hermes

#endif // HERMES_COSMICRAYDENSITY_H

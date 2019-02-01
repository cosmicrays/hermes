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
        typedef tEnergyRange::iterator iterator;
        typedef tEnergyRange::const_iterator const_iterator;

        virtual ~CosmicRayDensity() { }
        virtual QPDensityPerEnergy getDensityPerEnergy(
			const QEnergy& E_, const Vector3QLength& pos_) const = 0;
	std::size_t getIndexOfE(const QEnergy& E_) const {
		const_iterator it =
			std::find_if(begin(), end(),
				[E_](const auto &a) {return a == E_;});
		return std::distance(begin(), it);
	}
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

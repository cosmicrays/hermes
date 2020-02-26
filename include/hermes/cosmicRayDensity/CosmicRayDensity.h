#ifndef HERMES_COSMICRAYDENSITY_H
#define HERMES_COSMICRAYDENSITY_H

#include "hermes/Units.h"
#include "hermes/Grid.h"

#include <cassert>

namespace hermes {

class PID {
protected:
	int Z;
	int A;
	int id;
public:
	PID(int Z_, int A_) : Z(Z_), A(A_) {
		assert(A_ >= 0);
		id = Z * 100 + A;
	}

	int getID() const {
		return id;
	}

	virtual ~PID() {
	}

	bool isNucleus() {
		return (A > 0);
	}

	int atomicNr() const {
		return Z;
	}

	int massNr() const {
		return A;
	}
};

static const PID Electron(-1, 0);
static const PID Positron(1, 0);
static const PID Proton(1, 1);
static const PID Helium(2, 4);

class CosmicRayDensity {
protected:
	typedef std::vector<QEnergy> tEnergyRange;
	mutable tEnergyRange energyRange;
	bool scaleFactorFlag;
	double energyScaleFactor;
public:
        typedef tEnergyRange::iterator iterator;
        typedef tEnergyRange::const_iterator const_iterator;

	CosmicRayDensity(bool scaleFactorFlag_ = false) : scaleFactorFlag(scaleFactorFlag_) { }
        virtual ~CosmicRayDensity() { }

        virtual QPDensityPerEnergy getDensityPerEnergy(
			const QEnergy& E_, const Vector3QLength& pos_) const = 0;
	std::size_t getIndexOfE(const QEnergy& E_) const {
		const_iterator it = std::find_if(begin(), end(),
				[E_](const auto &a) {return a == E_;});
		return std::distance(begin(), it);
	}
	bool existsScaleFactor() const {
		return scaleFactorFlag;
	}
	double getEnergyScaleFactor() const {
		return energyScaleFactor;
	}

	tEnergyRange getEnergyAxis() const {
		return energyRange;
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

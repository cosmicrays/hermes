#ifndef HERMES_COSMICRAYDENSITY_H
#define HERMES_COSMICRAYDENSITY_H

#include <algorithm>
#include <cassert>
#include <set>

#include "hermes/Grid.h"
#include "hermes/ParticleID.h"
#include "hermes/Units.h"

namespace hermes { namespace cosmicrays {
/**
 * \addtogroup CosmicRays
 * @{
 */

class CosmicRayDensity {
  protected:
	typedef std::vector<QEnergy> tEnergyRange;
	mutable tEnergyRange energyRange;
	bool scaleFactorFlag;
	double energyScaleFactor;
	std::set<PID> setOfPIDs;

	void enablePID(const PID &pid_) { setOfPIDs.insert(pid_); }
	void disablePID(const PID &pid_) { setOfPIDs.erase(setOfPIDs.find(pid_)); }
	bool isPIDEnabled(const PID &pid_) const {
		return (setOfPIDs.count(pid_) > 0);
	}

  public:
	typedef tEnergyRange::iterator iterator;
	typedef tEnergyRange::const_iterator const_iterator;

	CosmicRayDensity(const PID &pid = Proton)
	    : scaleFactorFlag(true), setOfPIDs(std::set<PID>{pid}) {}

	CosmicRayDensity(const std::vector<PID> &pids_) : scaleFactorFlag(true) {
		for (auto const &p : pids_) enablePID(p);
	}
	virtual ~CosmicRayDensity() {}

	virtual QPDensityPerEnergy getDensityPerEnergy(
	    const QEnergy &E_, const Vector3QLength &pos_) const = 0;
	std::size_t getIndexOfE(const QEnergy &E_) const {
		const_iterator it = std::find_if(
		    begin(), end(), [E_](const auto &a) { return a == E_; });
		return std::distance(begin(), it);
	}
	bool existsScaleFactor() const { return scaleFactorFlag; }
	void setScaleFactor(bool b) { scaleFactorFlag = b; }
	double getEnergyScaleFactor() const { return energyScaleFactor; }

	tEnergyRange getEnergyAxis() const { return energyRange; }

	PID getPID() const { return *setOfPIDs.begin(); }

	iterator beginAfterEnergy(const QEnergy &E_) {
		return std::upper_bound(energyRange.begin(), energyRange.end(), E_);
	}
	const_iterator beginAfterEnergy(const QEnergy &E_) const {
		return std::upper_bound(energyRange.begin(), energyRange.end(), E_);
	}
	iterator begin() { return energyRange.begin(); }
	const_iterator begin() const { return energyRange.begin(); }
	iterator end() { return energyRange.end(); }
	const_iterator end() const { return energyRange.end(); }
};

/** @}*/
}}  // namespace hermes::cosmicrays

#endif  // HERMES_COSMICRAYDENSITY_H

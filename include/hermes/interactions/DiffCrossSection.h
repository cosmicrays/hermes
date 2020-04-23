#ifndef HERMES_DIFFERENTIALCROSSSECTION_H
#define HERMES_DIFFERENTIALCROSSSECTION_H

#include "hermes/ParticleID.h"
#include "hermes/Units.h"

namespace hermes { namespace interactions {

using namespace units;

class DifferentialCrossSection {
  protected:
	bool cachingEnabled;

  public:
	DifferentialCrossSection(bool cachingEnabled = false);
	~DifferentialCrossSection();

	void enableCaching();
	void disableCaching();

	virtual QDiffCrossSection getDiffCrossSection(const QEnergy &E_proton,
	                                              const QEnergy &E_gamma) const;
	virtual QDiffCrossSection getDiffCrossSection(const QEnergy &E_electron,
	                                              const QEnergy &E_photon,
	                                              const QEnergy &E_gamma) const;
	virtual QNumber getSigma(const PID &projectile, const PID &target) const;
};

}}  // namespace hermes::interactions

#endif  // HERMES_DIFFERENTIALCROSSSECTION_H

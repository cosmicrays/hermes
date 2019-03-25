#ifndef HERMES_SYNCHROABSORPTIONINTEGRATOR_H
#define HERMES_SYNCHROABSORPTIONINTEGRATOR_H

#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/integrators/FreeFreeIntegrator.h"

namespace hermes {

class SynchroAbsorptionIntegrator: public SynchroIntegrator, FreeFreeIntegrator {
public:
	SynchroAbsorptionIntegrator(
		const std::shared_ptr<MagneticField> mfield,
		const std::shared_ptr<CosmicRayDensity> crdensity,
		const std::shared_ptr<GasDensity> gdensity);
	~SynchroAbsorptionIntegrator();

	QTemperature integrateOverLOS(QDirection iterdir_) const;
	QTemperature integrateOverLOS(QDirection iterdir_, QFrequency freq_) const;
};

} // namespace hermes

#endif // HERMES_SYNCHROABSORPTIONINTEGRATOR_H

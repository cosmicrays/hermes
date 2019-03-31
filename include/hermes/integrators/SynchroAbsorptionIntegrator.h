#ifndef HERMES_SYNCHROABSORPTIONINTEGRATOR_H
#define HERMES_SYNCHROABSORPTIONINTEGRATOR_H

#include "hermes/integrators/Integrator.h"
#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/integrators/FreeFreeIntegrator.h"

namespace hermes {

class SynchroAbsorptionIntegrator: public IntegratorTemplate<QTemperature> {
private:
	std::shared_ptr<MagneticField> mfield;
	std::shared_ptr<CosmicRayDensity> crdensity;
	std::shared_ptr<GasDensity> gdensity;
	std::shared_ptr<SynchroIntegrator> intSynchro;
	std::shared_ptr<FreeFreeIntegrator> intFreeFree;

public:
	SynchroAbsorptionIntegrator(
		const std::shared_ptr<MagneticField> mfield,
		const std::shared_ptr<CosmicRayDensity> crdensity,
		const std::shared_ptr<GasDensity> gdensity);
	~SynchroAbsorptionIntegrator();

	QTemperature integrateOverLOS(QDirection iterdir_) const;
	QTemperature integrateOverLOS(QDirection iterdir, QFrequency freq) const;
};

} // namespace hermes

#endif // HERMES_SYNCHROABSORPTIONINTEGRATOR_H

#ifndef HERMES_SYNCHROABSORPTIONINTEGRATOR_H
#define HERMES_SYNCHROABSORPTIONINTEGRATOR_H

#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/integrators/FreeFreeIntegrator.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class SynchroAbsorptionIntegrator: public RadioIntegratorTemplate {
private:
	std::shared_ptr<MagneticField> mfield;
	std::shared_ptr<CosmicRayDensity> crdensity;
	std::shared_ptr<ChargedGasDensity> gdensity;
	std::shared_ptr<SynchroIntegrator> intSynchro;
	std::shared_ptr<FreeFreeIntegrator> intFreeFree;

public:
	SynchroAbsorptionIntegrator(
		const std::shared_ptr<MagneticField> mfield,
		const std::shared_ptr<CosmicRayDensity> crdensity,
		const std::shared_ptr<ChargedGasDensity> gdensity);
	~SynchroAbsorptionIntegrator();

	void setFrequency(const QFrequency &freq);
	QFrequency getFrequency() const;

	QTemperature integrateOverLOS(QDirection iterdir_) const;
	QTemperature integrateOverLOS(QDirection iterdir, QFrequency freq) const;
};

/** @}*/
} // namespace hermes

#endif // HERMES_SYNCHROABSORPTIONINTEGRATOR_H

#ifndef HERMES_GENERICINTEGRATOR_H
#define HERMES_GENERICINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/integrators/Integrator.h"
#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/integrators/FreeFreeIntegrator.h"
#include "hermes/neutralGasDensity/RingModelDensity.h"

#include <memory>

namespace hermes {

class GenericIntegrator: public IntegratorTemplate<QNumber, QFrequency> {
private:
	std::shared_ptr<MagneticField> mfield;
	std::shared_ptr<CosmicRayDensity> crdensity;
	std::shared_ptr<ChargedGasDensity> cgdensity;
	std::shared_ptr<RingModelDensity> ngdensity;
	std::shared_ptr<SynchroIntegrator> intSynchro;
	std::shared_ptr<FreeFreeIntegrator> intFreeFree;

public:
	GenericIntegrator(
		const std::shared_ptr<MagneticField> mfield,
		const std::shared_ptr<CosmicRayDensity> crdensity,
		const std::shared_ptr<ChargedGasDensity> gdensity,
		const std::shared_ptr<RingModelDensity> ndensity);
	~GenericIntegrator();
	QNumber integrateOverLOS(QDirection iterdir) const;
	QNumber integrateOverLOS(QDirection iterdir, QFrequency freq) const;
};

} // namespace hermes

#endif // HERMES_GENERICINTEGRATOR_H

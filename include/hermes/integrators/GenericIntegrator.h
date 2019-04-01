#ifndef HERMES_GENERICINTEGRATOR_H
#define HERMES_GENERICINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/integrators/Integrator.h"
#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/integrators/FreeFreeIntegrator.h"

#include <memory>

namespace hermes {

class GenericIntegrator: public IntegratorTemplate<QNumber> {
private:
	std::shared_ptr<MagneticField> mfield;
	std::shared_ptr<CosmicRayDensity> crdensity;
	std::shared_ptr<GasDensity> gdensity;
	std::shared_ptr<SynchroIntegrator> intSynchro;
	std::shared_ptr<FreeFreeIntegrator> intFreeFree;

public:
	GenericIntegrator(
		const std::shared_ptr<MagneticField> mfield,
		const std::shared_ptr<CosmicRayDensity> crdensity,
		const std::shared_ptr<GasDensity> gdensity);
	~GenericIntegrator();
	QNumber integrateOverLOS(QDirection iterdir) const;
	QNumber integrateOverLOS(QDirection iterdir, QFrequency freq) const;
};

} // namespace hermes

#endif // HERMES_GENERICINTEGRATOR_H

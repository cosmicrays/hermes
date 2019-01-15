#ifndef HERMES_SYNCHROINTEGRATOR_H
#define HERMES_SYNCHROINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/skymaps/SynchroSkymap.h"
#include "hermes/integrators/Integrator.h"
#include "hermes/magneticField/MagneticField.h"
#include "hermes/cosmicRayDensity/CosmicRayDensity.h"

#include <memory>
#include <array>

namespace hermes {

class SynchroIntegrator: public Integrator<SynchroSkymap> {
private:
	std::shared_ptr<MagneticField> mfield;
	std::shared_ptr<CosmicRayDensity> crdensity;
	QFrequency freq;
public:
	SynchroIntegrator(
		const std::shared_ptr<MagneticField> mfield,
		const std::shared_ptr<CosmicRayDensity> crdensity);
	~SynchroIntegrator();

	void set_output();
	SynchroSkymap::tPixel integrateOverLOS(QDirection iterdir);
	QTemperature intensityToTemperature(QIntensity intensity);
	QEmissivity integrateOverEnergy(Vector3QLength pos, QFrequency freq);
};

} // namespace hermes

#endif // HERMES_SYNCHROINTEGRATOR_H

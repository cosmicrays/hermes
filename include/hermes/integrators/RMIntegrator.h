#ifndef HERMES_RMINTEGRATOR_H
#define HERMES_RMINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/skymaps/RMSkymap.h"
#include "hermes/integrators/Integrator.h"
#include "hermes/magneticField/MagneticField.h"
#include "hermes/gasDensity/GasDensity.h"

#include <memory>
#include <array>

namespace hermes {

class RMIntegrator: public Integrator<RMSkymap> {
private:
	std::shared_ptr<MagneticField> mfield;
	std::shared_ptr<GasDensity> gdensity;
public:
	RMIntegrator(const std::shared_ptr<MagneticField> mfield, const std::shared_ptr<GasDensity> gdensity);
	~RMIntegrator();

	void set_output();
	RMSkymap::tPixel integral(tDirection iterdir);
};

} // namespace hermes

#endif // HERMES_RMINTEGRATOR_H

#ifndef HERMES_RMINTEGRATOR_H
#define HERMES_RMINTEGRATOR_H

#include "hermes/integrators/Integrator.h"
#include "hermes/magneticField/MagneticField.h"

#include <memory>
#include <array>

namespace hermes {

class RMIntegrator: public Integrator {
private:
	std::shared_ptr<MagneticField> mfield;
public:
	RMIntegrator(const std::shared_ptr<MagneticField> mfield);
	virtual ~RMIntegrator();

	void set_output();
	double integral(tDirection iterdir);
};

} // namespace hermes

#endif // HERMES_RMINTEGRATOR_H

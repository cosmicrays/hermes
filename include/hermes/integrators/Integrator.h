#ifndef HERMES_INTEGRATOR_H
#define HERMES_INTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/HEALPix.h"
#include "hermes/Skymap.h"

#include <array>

namespace hermes {

class Integrator {
private:
	std::shared_ptr<Skymap> working_skymap;
public:
	typedef std::array<QAngle, 2> tDirection;
	tDirection iterdir;

	Integrator();
        virtual ~Integrator();

	void set_skymap(std::shared_ptr<Skymap> skymap);
	virtual double integral(tDirection iterdir) = 0;
	void compute();
};

} // namespace hermes

#endif // HERMES_INTEGRATOR_H

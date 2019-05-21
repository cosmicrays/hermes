#ifndef HERMES_INTEGRATOR_H
#define HERMES_INTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/Grid.h"
#include "hermes/HEALPixBits.h"
#include "hermes/Common.h"

#include <memory>

namespace hermes {

template <class T>
class IntegratorTemplate {
public:
	IntegratorTemplate() { };

	/**
 		Every child class should implement this method which represents an integral
		of a targeted accumulated quantity `T` in a given direction `interdir`.
	*/
	virtual T integrateOverLOS(QDirection iterdir) const = 0;
	/**
		Additionally, for a frequency dependent integrals should implement
		the second method too.
	*/
	virtual T integrateOverLOS(QDirection iterdir, QFrequency) const { return T(0); };
	virtual T integrateOverLOS(QDirection iterdir, QEnergy) const { return T(0); };
};

} // namespace hermes

#endif // HERMES_INTEGRATOR_H

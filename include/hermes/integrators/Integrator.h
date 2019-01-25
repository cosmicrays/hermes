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
	inline IntegratorTemplate() { };

	/**
 		Every child class should implement this method which provides an expression
		of a given direction prepared for the integration
	*/
	virtual T integrateOverLOS(QDirection iterdir) = 0;
};

} // namespace hermes

#endif // HERMES_INTEGRATOR_H

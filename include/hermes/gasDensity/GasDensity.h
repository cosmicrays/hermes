#ifndef HERMES_GASDENSITY_H
#define HERMES_GASDENSITY_H

#include "hermes/Units.h"
#include "hermes/Grid.h"

namespace hermes {

class GasDensity {
public:
	virtual ~GasDensity() { }
	virtual QPDensity getDensity(const Vector3QLength& pos) const = 0;
};

} // namespace hermes

#endif // HERMES_GASDENSITY_H

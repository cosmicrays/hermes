#ifndef HERMES_GASDENSITY_H
#define HERMES_GASDENSITY_H

#include "hermes/Units.h"
#include "hermes/Grid.h"

namespace hermes {

class GasDensity {
private:
	QTemperature gasTemp;
public:
	GasDensity() { }
	GasDensity(QTemperature T) : gasTemp(T) { }
	virtual ~GasDensity() { }
	virtual QPDensity getDensity(const Vector3QLength& pos) const = 0;

	inline void setTemperature(QTemperature T) {
		gasTemp = T;
	}
	inline QTemperature getTemperature() const {
		return gasTemp;
	}
};

} // namespace hermes

#endif // HERMES_GASDENSITY_H

#ifndef HERMES_CHARGEDGASDENSITY_H
#define HERMES_CHARGEDGASDENSITY_H

#include "hermes/Units.h"
#include "hermes/Grid.h"

namespace hermes {

class ChargedGasDensity {
private:
	QTemperature gasTemp;
public:
	ChargedGasDensity() { }
	ChargedGasDensity(QTemperature T) : gasTemp(T) { }
	virtual ~ChargedGasDensity() { }
	virtual QPDensity getDensity(const Vector3QLength& pos) const = 0;

	inline void setTemperature(QTemperature T) {
		gasTemp = T;
	}
	inline QTemperature getTemperature() const {
		return gasTemp;
	}
};

} // namespace hermes

#endif // HERMES_CHARGEDGASDENSITY_H

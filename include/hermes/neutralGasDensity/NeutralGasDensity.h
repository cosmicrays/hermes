#ifndef HERMES_NEUTRALGASDENSITY_H
#define HERMES_NEUTRALGASDENSITY_H

#include "hermes/Units.h"
#include "hermes/Grid.h"

namespace hermes {

enum GasType { HI = 0, CO = 1 };

class NeutralGasDensity {
private:
	QTemperature gasTemp;
public:
	NeutralGasDensity() : gasTemp(1e4_K) { }
	NeutralGasDensity(QTemperature T) : gasTemp(T) { }

	virtual int getRingNumber(GasType t) const = 0;

	inline void setTemperature(QTemperature T) {
		gasTemp = T;
	}
	inline QTemperature getTemperature() const {
		return gasTemp;
	}
	
};

} // namespace hermes

#endif // HERMES_NEUTRALGASDENSITY_H

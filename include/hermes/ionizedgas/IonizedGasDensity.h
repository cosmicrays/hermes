#ifndef HERMES_IONIZEDGASDENSITY_H
#define HERMES_IONIZEDGASDENSITY_H

#include "hermes/Grid.h"
#include "hermes/Units.h"

namespace hermes { namespace ionizedgas {
/**
 * \addtogroup IonizedGas
 * @{
 */

class IonizedGasDensity {
  private:
	QTemperature gasTemp;

  public:
	IonizedGasDensity() : gasTemp(1e4_K) {}
	IonizedGasDensity(QTemperature T) : gasTemp(T) {}
	virtual ~IonizedGasDensity() {}
	virtual QPDensity getDensity(const Vector3QLength &pos) const = 0;

	inline void setTemperature(QTemperature T) { gasTemp = T; }
	inline QTemperature getTemperature() const { return gasTemp; }
};

/** @}*/
}}  // namespace hermes::ionizedgas

#endif  // HERMES_IONIZEDGASDENSITY_H

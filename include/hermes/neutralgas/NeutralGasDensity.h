#ifndef HERMES_NEUTRALGASDENSITY_H
#define HERMES_NEUTRALGASDENSITY_H

#include "hermes/Grid.h"
#include "hermes/Units.h"

namespace hermes { namespace neutralgas {
/**
 * \addtogroup NeutralGas
 * @{
 */

class NeutralGasDensity {
  private:
	QTemperature gasTemp;

  public:
	NeutralGasDensity() : gasTemp(1e4_K) {}
	NeutralGasDensity(QTemperature T) : gasTemp(T) {}

	virtual ~NeutralGasDensity() {}

	virtual int getRingNumber() const = 0;

	inline void setTemperature(QTemperature T) { gasTemp = T; }
	inline QTemperature getTemperature() const { return gasTemp; }
};

/** @}*/
}}  // namespace hermes::neutralgas

#endif  // HERMES_NEUTRALGASDENSITY_H

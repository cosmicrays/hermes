#ifndef HERMES_NEUTRALGASABSTRACT_H
#define HERMES_NEUTRALGASABSTRACT_H

#include "hermes/Grid.h"
#include "hermes/Units.h"

namespace hermes { namespace neutralgas {
/**
 * \addtogroup NeutralGas
 * @{
 */

class NeutralGasAbstract {
  private:
	QTemperature gasTemp;

  public:
	NeutralGasAbstract() : gasTemp(1e4_K) {}
	NeutralGasAbstract(QTemperature T) : gasTemp(T) {}

	virtual ~NeutralGasAbstract() {}

	virtual int getRingNumber() const = 0;

	inline void setTemperature(QTemperature T) { gasTemp = T; }
	inline QTemperature getTemperature() const { return gasTemp; }
};

/** @}*/
}}  // namespace hermes::neutralgas

#endif  // HERMES_NEUTRALGASABSTRACT_H

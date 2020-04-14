#ifndef HERMES_FREEFREEINTEGRATOR_H
#define HERMES_FREEFREEINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/magneticField/MagneticField.h"
#include "hermes/chargedgas/ChargedGasDensity.h"

#include <memory>
#include <array>

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class FreeFreeIntegrator: public RadioIntegratorTemplate {
private:
	std::shared_ptr<chargedgas::ChargedGasDensity> gdensity;

public:
	FreeFreeIntegrator(
		const std::shared_ptr<chargedgas::ChargedGasDensity> gdensity);
	~FreeFreeIntegrator();
	
	void setFrequency(const QFrequency &freq);
	QFrequency getFrequency() const;

	QTemperature integrateOverLOS(QDirection iterdir) const;
	QTemperature integrateOverLOS(QDirection iterdir, QFrequency freq) const;

	QNumber gauntFactor(
		QFrequency freq, QTemperature T, int Z) const;
	QEmissivity spectralEmissivityExplicit(
	        QPDensity N, QPDensity N_e, QFrequency freq, QTemperature T, int Z) const;
	QEmissivity spectralEmissivity(
		Vector3QLength pos, QFrequency freq) const;
	QInverseLength absorptionCoefficient(
		Vector3QLength pos, QFrequency freq) const;
	
};

/** @}*/
} // namespace hermes

#endif // HERMES_FREEFREEINTEGRATOR_H

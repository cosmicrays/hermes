#ifndef HERMES_FREEFREEINTEGRATOR_H
#define HERMES_FREEFREEINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/integrators/Integrator.h"
#include "hermes/magneticField/MagneticField.h"
#include "hermes/chargedGasDensity/ChargedGasDensity.h"

#include <memory>
#include <array>

namespace hermes {

class FreeFreeIntegrator: public IntegratorTemplate<QTemperature> {
private:
	std::shared_ptr<ChargedGasDensity> gdensity;

public:
	FreeFreeIntegrator(
		const std::shared_ptr<ChargedGasDensity> gdensity);
	~FreeFreeIntegrator();

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

} // namespace hermes

#endif // HERMES_FREEFREEINTEGRATOR_H

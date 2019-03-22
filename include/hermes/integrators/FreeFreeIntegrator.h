#ifndef HERMES_FREEFREEINTEGRATOR_H
#define HERMES_FREEFREEINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/integrators/Integrator.h"
#include "hermes/magneticField/MagneticField.h"
#include "hermes/gasDensity/GasDensity.h"

#include <memory>
#include <array>

namespace hermes {

class FreeFreeIntegrator: public IntegratorTemplate<QTemperature> {
private:
	std::shared_ptr<GasDensity> gdensity;

public:
	FreeFreeIntegrator(
		const std::shared_ptr<GasDensity> gdensity);
	~FreeFreeIntegrator();

	QTemperature integrateOverLOS(QDirection iterdir_) const;
	QTemperature integrateOverLOS(QDirection iterdir_, QFrequency freq_) const;

	QNumber gauntFactor(QFrequency freq, QTemperature T, int Z) const;
	QEmissivity spectralEmissivity(
	        QPDensity N, QPDensity N_e, QFrequency freq, QTemperature T, int Z) const;
};

} // namespace hermes

#endif // HERMES_FREEFREEINTEGRATOR_H

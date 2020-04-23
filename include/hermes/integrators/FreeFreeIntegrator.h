#ifndef HERMES_FREEFREEINTEGRATOR_H
#define HERMES_FREEFREEINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/chargedgas/ChargedGasDensity.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/magneticfields/MagneticField.h"

#include <array>
#include <memory>

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class FreeFreeIntegrator : public RadioIntegratorTemplate {
  private:
    std::shared_ptr<chargedgas::ChargedGasDensity> gdensity;

  public:
    FreeFreeIntegrator(
	const std::shared_ptr<chargedgas::ChargedGasDensity>& gdensity);
    ~FreeFreeIntegrator();

    void setFrequency(const QFrequency &freq);
    QFrequency getFrequency() const;

    QTemperature integrateOverLOS(const QDirection &iterdir) const override;
    QTemperature integrateOverLOS(const QDirection &iterdir, const QFrequency &freq) const override;

    QNumber gauntFactor(const QFrequency &freq, const QTemperature &T, int Z) const;
    QEmissivity spectralEmissivityExplicit(const QPDensity &N, const QPDensity &N_e,
					   const QFrequency &freq, const QTemperature &T,  int Z) const;
    QEmissivity spectralEmissivity(const Vector3QLength& pos, const QFrequency &freq) const;
    QInverseLength absorptionCoefficient(const Vector3QLength& pos,
					 const QFrequency &freq) const;
};

/** @}*/
} // namespace hermes

#endif // HERMES_FREEFREEINTEGRATOR_H

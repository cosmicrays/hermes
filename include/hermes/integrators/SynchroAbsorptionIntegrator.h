#ifndef HERMES_SYNCHROABSORPTIONINTEGRATOR_H
#define HERMES_SYNCHROABSORPTIONINTEGRATOR_H

#include "hermes/integrators/FreeFreeIntegrator.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/integrators/SynchroIntegrator.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class SynchroAbsorptionIntegrator : public RadioIntegratorTemplate {
  private:
    std::shared_ptr<magneticfields::MagneticField> mfield;
    std::shared_ptr<cosmicrays::CosmicRayDensity> crdensity;
    std::shared_ptr<chargedgas::ChargedGasDensity> gdensity;
    std::shared_ptr<SynchroIntegrator> intSynchro;
    std::shared_ptr<FreeFreeIntegrator> intFreeFree;

  public:
    SynchroAbsorptionIntegrator(
	const std::shared_ptr<magneticfields::MagneticField>& mfield,
	const std::shared_ptr<cosmicrays::CosmicRayDensity>& crdensity,
	const std::shared_ptr<chargedgas::ChargedGasDensity>& gdensity);
    ~SynchroAbsorptionIntegrator();

    void setFrequency(const QFrequency &freq);
    QFrequency getFrequency() const;

    QTemperature integrateOverLOS(const QDirection &iterdir_) const;
    QTemperature integrateOverLOS(const QDirection &iterdir, const QFrequency &freq) const;
};

/** @}*/
} // namespace hermes

#endif // HERMES_SYNCHROABSORPTIONINTEGRATOR_H

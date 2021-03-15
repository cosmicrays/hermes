#ifndef HERMES_SYNCHROABSORPTIONINTEGRATOR_H
#define HERMES_SYNCHROABSORPTIONINTEGRATOR_H

#include "hermes/integrators/FreeFreeIntegrator.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/integrators/SynchroIntegrator.h"

/** \file SynchroAbsorptionIntegrator.h
 *  Declares SynchroAbsorptionIntegrator
 */

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

/**
 * \class SynchroAbsorptionIntegrator
 * \brief Calculates synchrotron radiation (DM) over the LOS
 *
 * The integrator depends on a galactic magnetic field model, such as
 * magneticfields.JF12, a cosmic ray lepton distribution and
 * a nonrelativistic ionised gas
 */
class SynchroAbsorptionIntegrator : public RadioIntegratorTemplate {
  private:
	std::shared_ptr<magneticfields::MagneticField> mfield;
	std::shared_ptr<cosmicrays::CosmicRayDensity> crdensity;
	std::shared_ptr<ionizedgas::IonizedGasDensity> gdensity;
	std::shared_ptr<SynchroIntegrator> intSynchro;
	std::shared_ptr<FreeFreeIntegrator> intFreeFree;

  public:
	SynchroAbsorptionIntegrator(const std::shared_ptr<magneticfields::MagneticField> &mfield,
	                            const std::shared_ptr<cosmicrays::CosmicRayDensity> &crdensity,
	                            const std::shared_ptr<ionizedgas::IonizedGasDensity> &gdensity);
	~SynchroAbsorptionIntegrator();

	void setFrequency(const QFrequency &freq);
	QFrequency getFrequency() const;

	QTemperature integrateOverLOS(const QDirection &iterdir_) const override;
	QTemperature integrateOverLOS(const QDirection &iterdir, const QFrequency &freq) const override;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_SYNCHROABSORPTIONINTEGRATOR_H

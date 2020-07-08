#ifndef HERMES_SYNCHROINTEGRATOR_H
#define HERMES_SYNCHROINTEGRATOR_H

#include <array>
#include <memory>

#include "hermes/Units.h"
#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/magneticfields/MagneticField.h"

/** \file SynchroIntegrator.h
 *  Declares SynchroIntegrator
 */

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

/**
 * \class SynchroIntegrator
 * \brief Calculates synchrotron radiation over the LOS
 *
 * The integrator depends on a galactic magnetic field model, such as
 * magneticfields::JF12 and cosmic ray lepton distribution
 */
class SynchroIntegrator : public RadioIntegratorTemplate {
  private:
	std::shared_ptr<magneticfields::MagneticField> mfield;
	std::shared_ptr<cosmicrays::CosmicRayDensity> crdensity;
	const QSynchroConstant const_synchro =
	    std::sqrt(3) * pow<3>(e_plus) /
	    (8 * pi * pi * epsilon0 * c_light * m_electron);

	QEmissivity integrateOverSumEnergy(const Vector3QLength &pos,
	                                   const QFrequency &freq) const;
	QEmissivity integrateOverLogEnergy(const Vector3QLength &pos,
	                                   const QFrequency &freq) const;

  public:
	SynchroIntegrator(
	    const std::shared_ptr<magneticfields::MagneticField> &mfield,
	    const std::shared_ptr<cosmicrays::CosmicRayDensity> &crdensity);
	~SynchroIntegrator();

	void setFrequency(const QFrequency &freq);
	QFrequency getFrequency() const;

	QTemperature integrateOverLOS(const QDirection &iterdir) const override;
	QTemperature integrateOverLOS(const QDirection &iterdir,
	                              const QFrequency &freq) const override;

	QEnergy singleElectronEmission(const QFrequency &freq, const QEnergy &E,
	                               const QMField &B_perp) const;
	QEmissivity integrateOverEnergy(const Vector3QLength &pos,
	                                const QFrequency &freq) const;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_SYNCHROINTEGRATOR_H

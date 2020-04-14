#ifndef HERMES_SYNCHROINTEGRATOR_H
#define HERMES_SYNCHROINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/magneticField/MagneticField.h"
#include "hermes/cosmicrays/CosmicRayDensity.h"

#include <memory>
#include <array>

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class SynchroIntegrator: public RadioIntegratorTemplate {
private:
	std::shared_ptr<MagneticField> mfield;
	std::shared_ptr<cosmicrays::CosmicRayDensity> crdensity;
	const QSynchroConstant const_synchro =
		std::sqrt(3)*pow<3>(e_plus)/(8*pi*pi*epsilon0*c_light*m_electron);

	QEmissivity integrateOverSumEnergy(
		Vector3QLength pos, QFrequency freq) const;
	QEmissivity integrateOverLogEnergy(
		Vector3QLength pos, QFrequency freq) const;

public:
	SynchroIntegrator(
		const std::shared_ptr<MagneticField> mfield,
		const std::shared_ptr<cosmicrays::CosmicRayDensity> crdensity);
	~SynchroIntegrator();

	void setFrequency(const QFrequency &freq);
	QFrequency getFrequency() const;
	
	QTemperature integrateOverLOS(QDirection iterdir) const;
	QTemperature integrateOverLOS(QDirection iterdir, QFrequency freq) const;
	
	QEnergy singleElectronEmission(QFrequency freq, QEnergy E,
		QMField B_perp) const;	
	QEmissivity integrateOverEnergy(
		Vector3QLength pos, QFrequency freq) const;
};

/** @}*/
} // namespace hermes

#endif // HERMES_SYNCHROINTEGRATOR_H

#ifndef HERMES_SYNCHROINTEGRATOR_H
#define HERMES_SYNCHROINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/integrators/Integrator.h"
#include "hermes/magneticField/MagneticField.h"
#include "hermes/cosmicRayDensity/CosmicRayDensity.h"

#include <memory>
#include <array>

namespace hermes {

class SynchroIntegrator: public IntegratorTemplate<QTemperature> {
private:
	std::shared_ptr<MagneticField> mfield;
	std::shared_ptr<CosmicRayDensity> crdensity;
	const QSynchroConstant const_synchro =
		std::sqrt(3)*pow<3>(e_plus)/(8*pi*pi*epsilon0*c_light*m_electron);

public:
	SynchroIntegrator(
		const std::shared_ptr<MagneticField> mfield,
		const std::shared_ptr<CosmicRayDensity> crdensity);
	~SynchroIntegrator();

	QTemperature integrateOverLOS(QDirection iterdir_) const;
	QTemperature integrateOverLOS(QDirection iterdir_, QFrequency freq_) const;
	QTemperature intensityToTemperature(
		QIntensity intensity_, QFrequency freq_) const;
	QEmissivity integrateOverEnergy(
		Vector3QLength pos_, QFrequency freq_) const;
	QEmissivity integrateOverLogEnergy(
		Vector3QLength pos_, QFrequency freq_) const;
};

} // namespace hermes

#endif // HERMES_SYNCHROINTEGRATOR_H

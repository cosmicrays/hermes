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

	QEmissivity integrateOverSumEnergy(
		Vector3QLength pos, QFrequency freq) const;
	QEmissivity integrateOverLogEnergy(
		Vector3QLength pos, QFrequency freq) const;

public:
	SynchroIntegrator(
		const std::shared_ptr<MagneticField> mfield,
		const std::shared_ptr<CosmicRayDensity> crdensity);
	~SynchroIntegrator();

	QTemperature integrateOverLOS(QDirection iterdir) const;
	QTemperature integrateOverLOS(QDirection iterdir, QFrequency freq) const;
	QEnergy singleElectronEmission(QFrequency freq, QEnergy E,
		QMField B_perp) const;	
	QEmissivity integrateOverEnergy(
		Vector3QLength pos, QFrequency freq) const;
};

} // namespace hermes

#endif // HERMES_SYNCHROINTEGRATOR_H

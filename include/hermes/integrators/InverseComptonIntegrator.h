#ifndef HERMES_INVERSECOMPTONINTEGRATOR_H
#define HERMES_INVERSECOMPTONINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/integrators/Integrator.h"
#include "hermes/neutralGasDensity/RingModelDensity.h"
#include "hermes/cosmicRayDensity/CosmicRayDensity.h"
#include "hermes/interactions/KleinNishina.h"
#include "hermes/photonField/PhotonField.h"
#include "hermes/CacheTools.h"

#include <memory>
#include <array>

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class InverseComptonIntegrator: public IntegratorTemplate<QDifferentialFlux, QEnergy> {
private:
	std::shared_ptr<CosmicRayDensity> crdensity;
	std::shared_ptr<PhotonField> phdensity;
	std::shared_ptr<KleinNishina> crossSec; 

	bool cacheStoragePresent;
	std::shared_ptr<CacheStorageIC> cache; 
	QICOuterIntegral axialCacheForIoE(Vector3QLength pos, QEnergy Egamma) const;

	QICOuterIntegral integrateOverSumEnergy(
		Vector3QLength pos, QEnergy Egamma) const;
	QICOuterIntegral integrateOverLogEnergy(
		Vector3QLength pos, QEnergy Egamma) const;
public:
	InverseComptonIntegrator(
		const std::shared_ptr<CosmicRayDensity>,
		const std::shared_ptr<PhotonField>,
		const std::shared_ptr<KleinNishina>); 
	~InverseComptonIntegrator();

	void setCacheStorage(std::unique_ptr<CacheStorageIC> cache);

	QDifferentialFlux integrateOverLOS(QDirection iterdir) const;
	QDifferentialFlux integrateOverLOS(QDirection iterdir, QEnergy Egamma) const;
	QICOuterIntegral integrateOverEnergy(
		Vector3QLength pos, QEnergy Egamma) const;
	QICInnerIntegral integrateOverPhotonEnergy(Vector3QLength pos,
		QEnergy Egamma, QEnergy Eelectron) const;
};

/** @}*/
} // namespace hermes

#endif // HERMES_INVERSECOMPTONINTEGRATOR_H

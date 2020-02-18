#ifndef HERMES_INVERSECOMPTONINTEGRATOR_H
#define HERMES_INVERSECOMPTONINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/ProgressBar.h"
#include "hermes/integrators/IntegratorTemplate.h"
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

class InverseComptonIntegrator: public GammaIntegratorTemplate {
private:
	std::shared_ptr<CosmicRayDensity> crdensity;
	std::shared_ptr<PhotonField> phdensity;
	std::shared_ptr<KleinNishina> crossSec; 

	typedef Grid<QICOuterIntegral> ICCacheTable;
	std::shared_ptr<ICCacheTable> cacheTable;
	QICOuterIntegral getIOEfromCache(Vector3QLength, QEnergy) const;
	void computeCacheInThread(std::size_t start, std::size_t end,
			const QEnergy &Egamma, std::shared_ptr<ProgressBar> &p);

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
	
	void setEnergy(const QEnergy &Egamma);
	QEnergy getEnergy() const;

	QDifferentialIntensity integrateOverLOS(QDirection iterdir) const;
	QDifferentialIntensity integrateOverLOS(QDirection iterdir, QEnergy Egamma) const;
	QICOuterIntegral integrateOverEnergy(
		Vector3QLength pos, QEnergy Egamma) const;
	QICInnerIntegral integrateOverPhotonEnergy(Vector3QLength pos,
		QEnergy Egamma, QEnergy Eelectron) const;

       	void setupCacheTable(int N_x, int N_y, int N_z);
	void initCacheTable();
};

/** @}*/
} // namespace hermes

#endif // HERMES_INVERSECOMPTONINTEGRATOR_H

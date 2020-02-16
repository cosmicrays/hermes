#ifndef HERMES_PIZEROINTEGRATOR_H
#define HERMES_PIZEROINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/ProgressBar.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/neutralGasDensity/NeutralGasDensity.h"
#include "hermes/neutralGasDensity/RingModelDensity.h"
#include "hermes/cosmicRayDensity/CosmicRayDensity.h"
#include "hermes/interactions/DifferentialCrossSection.h"

#include <memory>
#include <array>

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class PiZeroIntegrator: public GammaIntegratorTemplate {
private:
	std::shared_ptr<CosmicRayDensity> crdensity;
	std::shared_ptr<RingModelDensity> ngdensity;
	std::shared_ptr<DifferentialCrossSection> crossSec; 
	
	typedef Grid<QPiZeroIntegral> ICCacheTable;
	std::shared_ptr<ICCacheTable> cacheTable;
	QPiZeroIntegral getIOEfromCache(Vector3QLength, QEnergy) const;
	void computeCacheInThread(std::size_t start, std::size_t end,
			const QEnergy &Egamma, std::shared_ptr<ProgressBar> &p);

	QPiZeroIntegral integrateOverSumEnergy(
		Vector3QLength pos, QEnergy Egamma) const;
	QPiZeroIntegral integrateOverLogEnergy(
		Vector3QLength pos, QEnergy Egamma) const;

public:
	PiZeroIntegrator(
		const std::shared_ptr<CosmicRayDensity>,
		const std::shared_ptr<RingModelDensity>,
		const std::shared_ptr<DifferentialCrossSection>); 
	~PiZeroIntegrator();

	QPDensity densityProfile(const Vector3QLength &) const;
	QRingX0Unit X0Function(const Vector3QLength &) const;
	
	QDifferentialIntensity integrateOverLOS(QDirection iterdir) const;
	QDifferentialIntensity integrateOverLOS(QDirection iterdir, QEnergy Egamma) const;
	QPiZeroIntegral integrateOverEnergy(
		Vector3QLength pos, QEnergy Egamma) const;

	void initCacheTable(QEnergy, int, int, int);
};

/** @}*/
} // namespace hermes

#endif // HERMES_PIZEROINTEGRATOR_H

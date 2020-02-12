#ifndef HERMES_PIZEROINTEGRATOR_H
#define HERMES_PIZEROINTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/integrators/Integrator.h"
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

class PiZeroIntegrator: public IntegratorTemplate<QDifferentialIntensity, QEnergy> {
private:
	std::shared_ptr<CosmicRayDensity> crdensity;
	std::shared_ptr<RingModelDensity> ngdensity;
	std::shared_ptr<DifferentialCrossSection> crossSec; 
	
	typedef Grid<QPiZeroIntegral> ICCacheTable;
	std::shared_ptr<ICCacheTable> cacheTable;
	bool cacheStoragePresent;
	QPiZeroIntegral getIOEfromCache(Vector3QLength, QEnergy) const;

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
	
	void initCacheTable(QEnergy, int, int, int);
	bool isCacheTableEnabled() const;

	QDifferentialIntensity integrateOverLOS(QDirection iterdir) const;
	QDifferentialIntensity integrateOverLOS(QDirection iterdir, QEnergy Egamma) const;
	QPiZeroIntegral integrateOverEnergy(
		Vector3QLength pos, QEnergy Egamma) const;
};

/** @}*/
} // namespace hermes

#endif // HERMES_PIZEROINTEGRATOR_H

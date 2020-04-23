#ifndef HERMES_PIZEROABSORPTIONINTEGRATOR_H
#define HERMES_PIZEROABSORPTIONINTEGRATOR_H

#include <array>
#include <memory>
#include <vector>

#include "hermes/ProgressBar.h"
#include "hermes/Units.h"
#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/interactions/DiffCrossSection.h"
#include "hermes/neutralgas/NeutralGasDensity.h"
#include "hermes/neutralgas/RingModel.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class PiZeroAbsorptionIntegrator : public GammaIntegratorTemplate {
  private:
	std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> crList;
	std::shared_ptr<neutralgas::RingModel> ngdensity;
	std::shared_ptr<interactions::DifferentialCrossSection> crossSec;

	typedef Grid<QPiZeroIntegral> ICCacheTable;
	std::shared_ptr<ICCacheTable> cacheTable;

	QPiZeroIntegral getIOEfromCache(const Vector3QLength &,
	                                const QEnergy &) const;
	void computeCacheInThread(std::size_t start, std::size_t end,
	                          const QEnergy &Egamma,
	                          std::shared_ptr<ProgressBar> &p);

  public:
	PiZeroAbsorptionIntegrator(
	    const std::shared_ptr<cosmicrays::CosmicRayDensity>,
	    const std::shared_ptr<neutralgas::RingModel>,
	    const std::shared_ptr<interactions::DifferentialCrossSection>);
	PiZeroAbsorptionIntegrator(
	    const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>>,
	    const std::shared_ptr<neutralgas::RingModel>,
	    const std::shared_ptr<interactions::DifferentialCrossSection>);
	~PiZeroAbsorptionIntegrator();

	void setEnergy(const QEnergy &Egamma);
	QEnergy getEnergy() const;

	QPDensity densityProfile(const Vector3QLength &) const;
	QRingX0Unit X0Function(const Vector3QLength &) const;

	QDiffIntensity integrateOverLOS(const QDirection &iterdir) const override;
	QDiffIntensity integrateOverLOS(const QDirection &iterdir,
	                                const QEnergy &Egamma) const override;

	QPiZeroIntegral integrateOverEnergy(const Vector3QLength &pos,
	                                    const QEnergy &Egamma) const;

	void setupCacheTable(int, int, int) override;
	void initCacheTable() override;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_PIZEROABSORPTIONINTEGRATOR_H

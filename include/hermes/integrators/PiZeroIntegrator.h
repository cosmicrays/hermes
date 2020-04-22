#ifndef HERMES_PIZEROINTEGRATOR_H
#define HERMES_PIZEROINTEGRATOR_H

#include "hermes/ProgressBar.h"
#include "hermes/Units.h"
#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/interactions/DifferentialCrossSection.h"
#include "hermes/neutralgas/NeutralGasDensity.h"
#include "hermes/neutralgas/RingModel.h"

#include <array>
#include <memory>
#include <vector>

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class PiZeroIntegrator : public GammaIntegratorTemplate {
  private:
    std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> crList;
    std::shared_ptr<neutralgas::RingModel> ngdensity;
    std::shared_ptr<interactions::DifferentialCrossSection> crossSec;

    typedef Grid<QPiZeroIntegral> ICCacheTable;
    std::shared_ptr<ICCacheTable> cacheTable;
    QPiZeroIntegral getIOEfromCache(Vector3QLength, QEnergy) const;
    void computeCacheInThread(std::size_t start, std::size_t end,
			      const QEnergy &Egamma,
			      std::shared_ptr<ProgressBar> &p);

  public:
    PiZeroIntegrator(
	const std::shared_ptr<cosmicrays::CosmicRayDensity>,
	const std::shared_ptr<neutralgas::RingModel>,
	const std::shared_ptr<interactions::DifferentialCrossSection>);
    PiZeroIntegrator(
	const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>>,
	const std::shared_ptr<neutralgas::RingModel>,
	const std::shared_ptr<interactions::DifferentialCrossSection>);
    ~PiZeroIntegrator();

    void setEnergy(const QEnergy &Egamma);
    QEnergy getEnergy() const;

    QPDensity densityProfile(const Vector3QLength &) const;
    QRingX0Unit X0Function(const Vector3QLength &) const;

    QDifferentialIntensity integrateOverLOS(QDirection iterdir) const;
    QDifferentialIntensity integrateOverLOS(QDirection iterdir,
					    QEnergy Egamma) const;

    QPiZeroIntegral integrateOverEnergy(Vector3QLength pos,
					QEnergy Egamma) const;

    void setupCacheTable(int, int, int);
    void initCacheTable();
};

/** @}*/
} // namespace hermes

#endif // HERMES_PIZEROINTEGRATOR_H

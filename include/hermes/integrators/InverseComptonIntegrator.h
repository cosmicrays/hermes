#ifndef HERMES_INVERSECOMPTONINTEGRATOR_H
#define HERMES_INVERSECOMPTONINTEGRATOR_H

#include "hermes/CacheTools.h"
#include "hermes/ProgressBar.h"
#include "hermes/Units.h"
#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/interactions/DiffCrossSection.h"
#include "hermes/photonfields/PhotonField.h"

#include <array>
#include <memory>

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class InverseComptonIntegrator : public GammaIntegratorTemplate {
  private:
    std::shared_ptr<cosmicrays::CosmicRayDensity> crdensity;
    std::shared_ptr<photonfields::PhotonField> phdensity;
    std::shared_ptr<interactions::DifferentialCrossSection> crossSec;

    typedef Grid<QGREmissivity> ICCacheTable;
    std::shared_ptr<ICCacheTable> cacheTable;
    QGREmissivity getIOEfromCache(Vector3QLength, QEnergy) const;
    void computeCacheInThread(std::size_t start, std::size_t end,
			      const QEnergy &Egamma,
			      std::shared_ptr<ProgressBar> &p);

    QGREmissivity integrateOverSumEnergy(Vector3QLength pos,
					 QEnergy Egamma) const;
    QGREmissivity integrateOverLogEnergy(Vector3QLength pos,
					 QEnergy Egamma) const;

  public:
    InverseComptonIntegrator(
	const std::shared_ptr<cosmicrays::CosmicRayDensity>,
	const std::shared_ptr<photonfields::PhotonField>,
	const std::shared_ptr<interactions::DifferentialCrossSection>);
    ~InverseComptonIntegrator();

    void setEnergy(const QEnergy &Egamma);
    QEnergy getEnergy() const;

    QDiffIntensity integrateOverLOS(QDirection iterdir) const;
    QDiffIntensity integrateOverLOS(QDirection iterdir, QEnergy Egamma) const;
    QGREmissivity integrateOverEnergy(Vector3QLength pos, QEnergy Egamma) const;
    QICInnerIntegral integrateOverPhotonEnergy(Vector3QLength pos,
					       QEnergy Egamma,
					       QEnergy Eelectron) const;

    void setupCacheTable(int N_x, int N_y, int N_z);
    void initCacheTable();
};

/** @}*/
} // namespace hermes

#endif // HERMES_INVERSECOMPTONINTEGRATOR_H

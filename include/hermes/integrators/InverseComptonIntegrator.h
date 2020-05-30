#ifndef HERMES_INVERSECOMPTONINTEGRATOR_H
#define HERMES_INVERSECOMPTONINTEGRATOR_H

#include <array>
#include <memory>

#include "hermes/CacheTools.h"
#include "hermes/ProgressBar.h"
#include "hermes/Units.h"
#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/interactions/DiffCrossSection.h"
#include "hermes/photonfields/PhotonField.h"

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
	QGREmissivity getIOEfromCache(const Vector3QLength &,
	                              const QEnergy &) const;
	void computeCacheInThread(std::size_t start, std::size_t end,
	                          const QEnergy &Egamma,
	                          std::shared_ptr<ProgressBar> &p);

	QGREmissivity integrateOverSumEnergy(const Vector3QLength &pos,
	                                     const QEnergy &Egamma) const;
	QGREmissivity integrateOverLogEnergy(const Vector3QLength &pos,
	                                     const QEnergy &Egamma) const;

  public:
	InverseComptonIntegrator(
	    const std::shared_ptr<cosmicrays::CosmicRayDensity> &,
	    const std::shared_ptr<photonfields::PhotonField> &,
	    const std::shared_ptr<interactions::DifferentialCrossSection> &);
	~InverseComptonIntegrator();

	void setEnergy(const QEnergy &Egamma);
	QEnergy getEnergy() const;

	QDiffIntensity integrateOverLOS(const QDirection &iterdir) const override;
	QDiffIntensity integrateOverLOS(const QDirection &iterdir,
	                                const QEnergy &Egamma) const override;
	QGREmissivity integrateOverEnergy(const Vector3QLength &pos,
	                                  const QEnergy &Egamma) const;
	QICInnerIntegral integrateOverPhotonEnergy(const Vector3QLength &pos,
	                                           const QEnergy &Egamma,
	                                           const QEnergy &Eelectron) const;

	void setupCacheTable(int N_x, int N_y, int N_z) override;
	void initCacheTable() override;
	
	tLOSProfile getLOSProfile(const QDirection &direction, const QEnergy &Egamma, int Nsteps) const override;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_INVERSECOMPTONINTEGRATOR_H

#ifndef HERMES_PIZEROINTEGRATOR_H
#define HERMES_PIZEROINTEGRATOR_H

#include <array>
#include <memory>
#include <utility>
#include <vector>

#include "hermes/ProgressBar.h"
#include "hermes/Units.h"
#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/interactions/DiffCrossSection.h"
#include "hermes/neutralgas/Nakanishi06.h"
#include "hermes/neutralgas/ProfileAbstract.h"
#include "hermes/neutralgas/RingModel.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class PiZeroIntegrator : public GammaIntegratorTemplate {
  protected:
	using LOSSegment = std::pair<QLength, QLength>;

	std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> crList;
	std::shared_ptr<neutralgas::RingModel> ngdensity;
	std::shared_ptr<interactions::DifferentialCrossSection> crossSec;

	std::shared_ptr<neutralgas::ProfileAbstract> dProfile;

	typedef Grid<QPiZeroIntegral> tCacheTable;
	std::shared_ptr<tCacheTable> cacheTable;
	unsigned int losIntegrationSteps;

	QPiZeroIntegral getIOEfromCache(const Vector3QLength &,
	                                const QEnergy &) const;
	std::vector<LOSSegment> getRingLOSIntervals(
	    const neutralgas::Ring &, const QDirection &) const;
	QDiffIntensity integrateOverLOSWithAttenuation(
	    const QDirection &, const QEnergy &, const QInverseLength &) const;
	void computeCacheInThread(std::size_t start, std::size_t end,
	                          const QEnergy &Egamma,
	                          std::shared_ptr<ProgressBar> &p);

  public:
	static constexpr unsigned int DefaultLOSIntegrationSteps = 500;

	PiZeroIntegrator(
	    const std::shared_ptr<cosmicrays::CosmicRayDensity> &,
	    const std::shared_ptr<neutralgas::RingModel> &,
	    const std::shared_ptr<interactions::DifferentialCrossSection> &);
	PiZeroIntegrator(
	    const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> &,
	    const std::shared_ptr<neutralgas::RingModel> &,
	    const std::shared_ptr<interactions::DifferentialCrossSection> &);
	virtual ~PiZeroIntegrator();

	void setEnergy(const QEnergy &Egamma);
	QEnergy getEnergy() const;

	QDiffIntensity integrateOverLOS(const QDirection &iterdir) const override;
	QDiffIntensity integrateOverLOS(const QDirection &iterdir,
	                                const QEnergy &Egamma) const override;

	virtual QPiZeroIntegral integrateOverEnergy(const Vector3QLength &pos,
	                                            const QEnergy &Egamma) const;
	/**
	 * Set the target number of composite-Simpson subintervals per gas ring.
	 * The budget is distributed over the exact LOS segments and rounded up to
	 * an even number on each segment.
	 */
	void setLOSIntegrationSteps(unsigned int steps);
	/** Return the target LOS integration-step budget per gas ring. */
	unsigned int getLOSIntegrationSteps() const;

	void setupCacheTable(int, int, int) override;
	void initCacheTable() override;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_PIZEROINTEGRATOR_H

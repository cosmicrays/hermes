#ifndef HERMES_PIZERONEUTRONDECAYINTEGRATOR_H
#define HERMES_PIZERONEUTRONDECAYINTEGRATOR_H

#include <array>
#include <memory>
#include <vector>

#include "hermes/ProgressBar.h"
#include "hermes/Units.h"
#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/interactions/DiffCrossSection.h"
#include "hermes/neutralgas/RingModel.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class PiZeroNeutronDecayIntegrator : public PiZeroIntegrator {
  public:
	PiZeroNeutronDecayIntegrator(const std::shared_ptr<cosmicrays::CosmicRayDensity> &,
	                             const std::shared_ptr<neutralgas::RingModel> &,
	                             const std::shared_ptr<interactions::DifferentialCrossSection> &);
	PiZeroNeutronDecayIntegrator(const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> &,
	                             const std::shared_ptr<neutralgas::RingModel> &,
	                             const std::shared_ptr<interactions::DifferentialCrossSection> &);
	~PiZeroNeutronDecayIntegrator();

	QDiffIntensity integrateOverLOS(const QDirection &iterdir) const override;
	QDiffIntensity integrateOverLOS(const QDirection &iterdir, const QEnergy &Egamma) const override;

	QInverseLength decayInverseLength(const QEnergy &Egamma) const;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_PIZERONEUTRONDECAYINTEGRATOR_H

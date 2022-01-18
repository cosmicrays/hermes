#ifndef HERMES_PIZEROABSORPTIONINTEGRATOR_H
#define HERMES_PIZEROABSORPTIONINTEGRATOR_H

#include <array>
#include <memory>
#include <vector>

#include "hermes/ProgressBar.h"
#include "hermes/Units.h"
#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/interactions/BreitWheeler.h"
#include "hermes/interactions/DiffCrossSection.h"
#include "hermes/neutralgas/RingModel.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class PiZeroAbsorptionIntegrator : public PiZeroIntegrator {
  private:
	std::unique_ptr<interactions::BreitWheeler> bwCrossSec{std::make_unique<interactions::BreitWheeler>()};

  public:
	PiZeroAbsorptionIntegrator(const std::shared_ptr<cosmicrays::CosmicRayDensity> &,
	                           const std::shared_ptr<neutralgas::RingModel> &,
	                           const std::shared_ptr<interactions::DifferentialCrossSection> &);
	PiZeroAbsorptionIntegrator(const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> &,
	                           const std::shared_ptr<neutralgas::RingModel> &,
	                           const std::shared_ptr<interactions::DifferentialCrossSection> &);
	~PiZeroAbsorptionIntegrator();

	QDiffIntensity integrateOverLOS(const QDirection &iterdir) const override;
	QDiffIntensity integrateOverLOS(const QDirection &iterdir, const QEnergy &Egamma) const override;

	QInverseLength absorptionCoefficient(const QEnergy &Egamma) const;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_PIZEROABSORPTIONINTEGRATOR_H

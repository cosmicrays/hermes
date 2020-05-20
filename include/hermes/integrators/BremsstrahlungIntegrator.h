#ifndef HERMES_BREMSSTRAHLUNGINTEGRATOR_H
#define HERMES_BREMSSTRAHLUNGINTEGRATOR_H

#include <array>
#include <memory>
#include <vector>

#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/interactions/BremsstrahlungSimple.h"

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

class BremsstrahlungIntegrator : public PiZeroIntegrator {
  private:
	std::shared_ptr<interactions::BremsstrahlungSimple> crossSec;
  public:
	BremsstrahlungIntegrator(
	    const std::shared_ptr<cosmicrays::CosmicRayDensity> &,
	    const std::shared_ptr<neutralgas::RingModel> &,
	    const std::shared_ptr<interactions::BremsstrahlungSimple> &);
	BremsstrahlungIntegrator(
	    const std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> &,
	    const std::shared_ptr<neutralgas::RingModel> &,
	    const std::shared_ptr<interactions::BremsstrahlungSimple> &);
	~BremsstrahlungIntegrator();

	QPiZeroIntegral integrateOverEnergy(const Vector3QLength &pos,
	                                    const QEnergy &Egamma) const override;
};

/** @}*/
}  // namespace hermes

#endif  // HERMES_BREMSSTRAHLUNGINTEGRATOR_H

#ifndef HERMES_BREMSSTRAHLUNGABSTRACT_H
#define HERMES_BREMSSTRAHLUNGABSTRACT_H

#include "hermes/interactions/DiffCrossSection.h"

namespace hermes { namespace interactions {

class BremsstrahlungAbstract : public DifferentialCrossSection {
  public:
	constexpr static std::size_t Ntargets = 3;
	enum class Target { HII, HI, He };
	const std::array<Target, Ntargets> allTargets = {
	    // to iterate over targets
	    Target::HII, Target::HI, Target::He};

	BremsstrahlungAbstract(){};
	virtual ~BremsstrahlungAbstract(){};

	virtual QDiffCrossSection getDiffCrossSectionForTarget(
	    Target t, const QEnergy &T_electron, const QEnergy &E_gamma) const = 0;
};

}}  // namespace hermes::interactions

#endif  // HERMES_BREMSSTRAHLUNGABSTRACT_H

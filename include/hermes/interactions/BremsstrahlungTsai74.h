#ifndef HERMES_BREMSSTRAHLUNGTSAI74_H
#define HERMES_BREMSSTRAHLUNGTSAI74_H

#include "hermes/interactions/BremsstrahlungAbstract.h"

namespace hermes { namespace interactions {
/**
 * \addtogroup Interactions
 * @{
 */

// Rev. Mod. Phys. 46, 815 (1974).
class BremsstrahlungTsai74 : public BremsstrahlungAbstract {
  private:
	std::pair<double, double> RadiationLogarithms(int Z) const;
	double computeCoulombCorrection(int Z, std::size_t n_max = 100) const;

  public:
	BremsstrahlungTsai74();

	QDiffCrossSection getDiffCrossSectionForTarget(
	    Target t, const QEnergy &T_electron,
	    const QEnergy &E_gamma) const override;
};

/** @}*/
}}  // namespace hermes::interactions

#endif  // HERMES_BREMSSTRAHLUNGTSAI74_H

#ifndef HERMES_KELNERAHARONIANNEUTRINO_H
#define HERMES_KELNERAHARONIANNEUTRINO_H

/*
Model from Phys.Rev.D74:034018,2006; Erratum-ibid.D79:039901,2009
    arXiv:astro-ph/0606058
*/

#include <memory>

#include "hermes/interactions/DiffCrossSection.h"
#include "hermes/interactions/KelnerAharonianGamma.h"

namespace hermes { namespace interactions {
/**
 * \addtogroup Interactions
 * @{
 */

class KelnerAharonianNeutrino : public DifferentialCrossSection {
  public:
	KelnerAharonianNeutrino();
	QDiffCrossSection getDiffCrossSection(const QEnergy &E_proton, const QEnergy &E_nu) const override;
	QDiffCrossSection getDiffCrossSection(const PID &projectile, const PID &target, const QEnergy &E_proj,
	                                      const QEnergy &E_secondary) const override;
};

/** @}*/
}}  // namespace hermes::interactions

#endif  // HERMES_KELNERAHARONIANNEUTRINO_H

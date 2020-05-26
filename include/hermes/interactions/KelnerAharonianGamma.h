#ifndef HERMES_KELNERAHARONIANGAMMA_H
#define HERMES_KELNERAHARONIANGAMMA_H

/*
Model from Phys.Rev.D74:034018,2006; Erratum-ibid.D79:039901,2009
    arXiv:astro-ph/0606058
*/

#include <memory>

#include "hermes/interactions/DiffCrossSection.h"

namespace hermes { namespace interactions {
/**
 * \addtogroup Interactions
 * @{
 */

class KelnerAharonianGamma : public DifferentialCrossSection {
  public:
	KelnerAharonianGamma();
	QDiffCrossSection getDiffCrossSection(
	    const QEnergy &E_proton, const QEnergy &E_gamma) const override;

	// Parametrization based on
	// Phys.Rev. D90 (2014) 12, 123014 (astro-ph/1406.7369)
	static QArea sigmaInelastic(const QEnergy &Tp);
};

/** @}*/
}}  // namespace hermes::interactions

#endif  // HERMES_KELNERAHARONIANGAMMA_H

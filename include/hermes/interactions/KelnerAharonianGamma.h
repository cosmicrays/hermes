#ifndef HERMES_KELNERAHARONIANGAMMA_H
#define HERMES_KELNERAHARONIANGAMMA_H

/*
Model from Phys.Rev.D74:034018,2006; Erratum-ibid.D79:039901,2009
	arXiv:astro-ph/0606058
*/

#include "hermes/interactions/DifferentialCrossSection.h"

#include <memory>

namespace hermes { namespace interactions {

class KelnerAharonianGamma: public DifferentialCrossSection {
public:
	KelnerAharonianGamma();
	QDifferentialCrossSection getDiffCrossSection(
			const QEnergy &E_proton, const QEnergy &E_gamma) const override;
	QArea sigmaInelastic(const QEnergy &Tp) const;
};

} // namespace interactions
} // namespace hermes

#endif // HERMES_KELNERAHARONIANGAMMA_H

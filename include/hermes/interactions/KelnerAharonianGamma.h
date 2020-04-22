#ifndef HERMES_KELNERAHARONIANGAMMA_H
#define HERMES_KELNERAHARONIANGAMMA_H

/*
Model from Phys.Rev.D74:034018,2006; Erratum-ibid.D79:039901,2009
	arXiv:astro-ph/0606058
*/

#include "hermes/interactions/DifferentialCrossSection.h"

#include <memory>

namespace hermes {
namespace interactions {

class KelnerAharonianGamma : public DifferentialCrossSection {
  public:
    KelnerAharonianGamma();
    QDifferentialCrossSection
    getDiffCrossSection(const QEnergy &E_proton,
			const QEnergy &E_gamma) const override;

    // Parametrization based on
    // Phys.Rev. D90 (2014) 12, 123014 (astro-ph/1406.7369)
    static QArea sigmaInelastic(const QEnergy &Tp);
};

} // namespace interactions
} // namespace hermes

#endif // HERMES_KELNERAHARONIANGAMMA_H

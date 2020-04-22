#ifndef HERMES_KELNERAHARONIANNEUTRINO_H
#define HERMES_KELNERAHARONIANNEUTRINO_H

/*
Model from Phys.Rev.D74:034018,2006; Erratum-ibid.D79:039901,2009
	arXiv:astro-ph/0606058
*/

#include "hermes/interactions/DifferentialCrossSection.h"
#include "hermes/interactions/KelnerAharonianGamma.h"

#include <memory>

namespace hermes {
namespace interactions {

class KelnerAharonianNeutrino : public DifferentialCrossSection {
  public:
    KelnerAharonianNeutrino();
    QDifferentialCrossSection
    getDiffCrossSection(const QEnergy &E_proton,
			const QEnergy &E_nu) const override;
};

} // namespace interactions
} // namespace hermes

#endif // HERMES_KELNERAHARONIANNEUTRINO_H

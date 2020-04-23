#ifndef HERMES_KLEINNISHINA_H
#define HERMES_KLEINNISHINA_H

#include <memory>

#include "hermes/interactions/DiffCrossSection.h"

namespace hermes { namespace interactions {

class KleinNishina : public DifferentialCrossSection {
  public:
	KleinNishina();

	QDiffCrossSection getDiffCrossSection(
	    const QEnergy &E_electron, const QEnergy &E_photon,
	    const QEnergy &E_gamma) const override;
};

}}  // namespace hermes::interactions

#endif  // HERMES_KLEINNISHINA_H

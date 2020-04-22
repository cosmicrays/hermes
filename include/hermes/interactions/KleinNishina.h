#ifndef HERMES_KLEINNISHINA_H
#define HERMES_KLEINNISHINA_H

#include "hermes/interactions/DifferentialCrossSection.h"

#include <memory>

namespace hermes {
namespace interactions {

class KleinNishina : public DifferentialCrossSection {
      public:
	KleinNishina();

	QDifferentialCrossSection
	getDiffCrossSection(const QEnergy &E_electron, const QEnergy &E_photon,
			    const QEnergy &E_gamma) const override;
};

} // namespace interactions
} // namespace hermes

#endif // HERMES_KLEINNISHINA_H

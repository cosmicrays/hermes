#ifndef HERMES_KLEINNISHINA_H
#define HERMES_KLEINNISHINA_H

#include "hermes/interactions/DifferentialCrossSection.h"

namespace hermes {

class KleinNishina: public DifferentialCrossSection {
public:
	KleinNishina();
	// not used
	QDiffCrossSection getDiffCrossSection(
			const QEnergy &E_photon,
			const QEnergy &E_gamma) const;

	QDiffCrossSection getDiffCrossSection(
			const QEnergy &E_electron,
			const QEnergy &E_photon,
			const QEnergy &E_gamma) const;
};

} // namespace hermes

#endif // HERMES_KLEINNISHINA_H

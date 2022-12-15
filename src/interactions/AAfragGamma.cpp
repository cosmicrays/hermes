#include "hermes/interactions/AAfragGamma.h"

#include <cmath>

namespace hermes { namespace interactions {

AAfragGamma::AAfragGamma() : DifferentialCrossSection() {}

QDiffCrossSection AAfragGamma::getDiffCrossSection(const QEnergy &E_proton, const QEnergy &E_gamma) const {
	if (E_gamma > E_proton) return QDiffCrossSection(0);

	return QDiffCrossSection(0.);
}

}}  // namespace hermes::interactions

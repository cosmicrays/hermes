#ifndef HERMES_DIFFERENTIALCROSSSECTION_H
#define HERMES_DIFFERENTIALCROSSSECTION_H

#include "hermes/Units.h"

namespace hermes {

class DifferentialCrossSection {
public:
	DifferentialCrossSection() { }
	~DifferentialCrossSection() { }
	virtual QDiffCrossSection getDiffCrossSection(
			const QEnergy &E_proton,
			const QEnergy &E_gamma) const = 0;
};

} // namespace hermes

#endif // HERMES_DIFFERENTIALCROSSSECTION_H

#ifndef HERMES_DIFFERENTIALCROSSSECTION_H
#define HERMES_DIFFERENTIALCROSSSECTION_H

#include "hermes/Units.h"

namespace hermes {

class DifferentialCrossSection {
public:
	DifferentialCrossSection() { }
	~DifferentialCrossSection() { }
	virtual QDifferentialCrossSection getDiffCrossSection(
			const QEnergy &E_proton,
			const QEnergy &E_gamma) const = 0;
        virtual QDifferentialCrossSection getDiffCrossSection(
                        const QEnergy &E_electron,
                        const QEnergy &E_photon,
                        const QEnergy &E_gamma) const = 0;
};

} // namespace hermes

#endif // HERMES_DIFFERENTIALCROSSSECTION_H

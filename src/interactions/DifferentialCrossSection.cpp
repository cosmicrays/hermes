#include "hermes/interactions/DiffCrossSection.h"

namespace hermes {
namespace interactions {

DifferentialCrossSection::DifferentialCrossSection(bool cachingEnabled_)
    : cachingEnabled(cachingEnabled_) {}

DifferentialCrossSection::~DifferentialCrossSection() {}

void DifferentialCrossSection::enableCaching() { cachingEnabled = true; };

void DifferentialCrossSection::disableCaching() { cachingEnabled = false; };

QDiffCrossSection
DifferentialCrossSection::getDiffCrossSection(const QEnergy &E_proton,
					      const QEnergy &E_gamma) const {
    return 0;
}

QDiffCrossSection
DifferentialCrossSection::getDiffCrossSection(const QEnergy &E_electron,
					      const QEnergy &E_photon,
					      const QEnergy &E_gamma) const {
    return 0;
}

QNumber DifferentialCrossSection::getSigma(const PID &projectile,
					   const PID &target) const {
    if (projectile == Proton && target == Proton)
	return 1.0_num;
    if (projectile == Proton && target == Helium)
	return 3.81_num;
    if (projectile == Helium && target == Proton)
	return 3.68_num;
    if (projectile == Helium && target == Helium)
	return 14.2_num;
    return 0.0_num;
}

} // namespace interactions
} // namespace hermes

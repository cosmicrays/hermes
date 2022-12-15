#include "hermes/interactions/DiffCrossSection.h"

namespace hermes { namespace interactions {

DifferentialCrossSection::DifferentialCrossSection(bool cachingEnabled_) : cachingEnabled(cachingEnabled_) {}

DifferentialCrossSection::~DifferentialCrossSection() {}

void DifferentialCrossSection::enableCaching() { cachingEnabled = true; };

void DifferentialCrossSection::disableCaching() { cachingEnabled = false; };

QDiffCrossSection DifferentialCrossSection::getDiffCrossSection(const QEnergy &E_proton, const QEnergy &E_gamma) const {
	return 0;
}

QDiffCrossSection DifferentialCrossSection::getDiffCrossSection(const QEnergy &E_electron, const QEnergy &E_photon,
                                                                const QEnergy &E_gamma) const {
	return 0;
}

QDiffCrossSection DifferentialCrossSection::getAADiffCrossSection(const PID &projectile, const PID &target,
                                                                  const QEnergy &E_proton,
                                                                  const QEnergy &E_gamma) const {
	if (projectile == Proton && target == Proton) return 1.0_num * getDiffCrossSection(E_proton, E_gamma);
	if (projectile == Proton && target == Helium) return 3.81_num * getDiffCrossSection(E_proton, E_gamma);
	if (projectile == Helium && target == Proton) return 3.68_num * getDiffCrossSection(E_proton, E_gamma);
	if (projectile == Helium && target == Helium) return 14.2_num * getDiffCrossSection(E_proton, E_gamma);
	return QDiffCrossSection(0);
}

}}  // namespace hermes::interactions

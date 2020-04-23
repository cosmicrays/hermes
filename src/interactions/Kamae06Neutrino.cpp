#include "hermes/interactions/Kamae06Neutrino.h"

namespace hermes {
namespace interactions {

Kamae06Neutrino::Kamae06Neutrino() : DifferentialCrossSection(false) {}

void Kamae06Neutrino::setCachingStorage(
    std::unique_ptr<CacheStorageCrossSection> cache_) {
    cache = std::move(cache_);
    enableCaching();
    auto f = [this](QEnergy E_proton, QEnergy E_nu) {
	return this->getDiffCrossSectionDirectly(E_proton, E_nu);
    };
    cache->setFunction(f);
};

QDiffCrossSection
Kamae06Neutrino::getDiffCrossSection(const QEnergy &E_proton,
				     const QEnergy &E_nu) const {
    if (cachingEnabled)
	return cache->getValue(E_proton, E_nu);
    return getDiffCrossSectionDirectly(E_proton, E_nu);
}

QDiffCrossSection
Kamae06Neutrino::getDiffCrossSectionDirectly(const QEnergy &E_proton,
					     const QEnergy &E_nu) const {

    PARAMSET params; /* struct where parameters are stored */
    // memset(&params, 0, sizeof(PARAMSET));

    /* proton kinetic energy in GeV */
    double T_proton_GeV = static_cast<double>(E_proton / 1_GeV);
    /* photon energ in GeV */
    double E_nu_GeV = static_cast<double>(E_nu / 1_GeV);

    double dsigma_dlogTp = 0;
    nue_param(T_proton_GeV, &params);
    dsigma_dlogTp += sigma_incl_tot(ID_NUE, E_nu_GeV, T_proton_GeV, &params);
    numu_param(T_proton_GeV, &params);
    dsigma_dlogTp += sigma_incl_tot(ID_NUMU, E_nu_GeV, T_proton_GeV, &params);
    antinue_param(T_proton_GeV, &params);
    dsigma_dlogTp +=
	sigma_incl_tot(ID_ANTINUE, E_nu_GeV, T_proton_GeV, &params);
    antinumu_param(T_proton_GeV, &params);
    dsigma_dlogTp +=
	sigma_incl_tot(ID_ANTINUMU, E_nu_GeV, T_proton_GeV, &params);

    return QDiffCrossSection(dsigma_dlogTp / E_nu_GeV * (1_mbarn / 1_GeV));
}

} // namespace interactions
} // namespace hermes

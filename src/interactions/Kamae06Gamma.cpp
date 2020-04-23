#include "hermes/interactions/Kamae06Gamma.h"

namespace hermes { namespace interactions {

Kamae06Gamma::Kamae06Gamma() : DifferentialCrossSection(false) {}

void Kamae06Gamma::setCachingStorage(
    std::unique_ptr<CacheStorageCrossSection> cache_) {
	cache = std::move(cache_);
	enableCaching();
	auto f = [this](QEnergy E_proton, QEnergy E_gamma) {
		return this->getDiffCrossSectionDirectly(E_proton, E_gamma);
	};
	cache->setFunction(f);
};

QDiffCrossSection Kamae06Gamma::getDiffCrossSection(
    const QEnergy &E_proton, const QEnergy &E_gamma) const {
	if (cachingEnabled) return cache->getValue(E_proton, E_gamma);
	return getDiffCrossSectionDirectly(E_proton, E_gamma);
}

QDiffCrossSection Kamae06Gamma::getDiffCrossSectionDirectly(
    const QEnergy &E_proton, const QEnergy &E_gamma) const {
	PARAMSET params; /* struct where parameters are stored */
	// memset(&params, 0, sizeof(PARAMSET));

	/* proton kinetic energy in GeV */
	double T_proton_GeV = static_cast<double>(E_proton / 1_GeV);
	/* photon energ in GeV */
	double E_gamma_GeV = static_cast<double>(E_gamma / 1_GeV);

	gamma_param(T_proton_GeV, &params);
	double dsigma_dlogTp = sigma_incl_tot(ID_GAMMA, E_gamma_GeV,
	                                      std::min(T_proton_GeV, 1e5), &params);

	return QDiffCrossSection(dsigma_dlogTp / E_gamma_GeV * (1_mbarn / 1_GeV));
}

}}  // namespace hermes::interactions

#include "hermes/interactions/Kamae06.h"

namespace hermes {

Kamae06::Kamae06() {
	cacheStoragePresent = false;
}

void Kamae06::setCacheStorage(std::unique_ptr<CacheStorageKamae> cache_) {
	cache = std::move(cache_);	
	cacheStoragePresent = true;
	auto f = [this](QEnergy E_proton, QEnergy E_gamma) {
			return this->getDiffCrossSectionDirectly(E_proton, E_gamma); };
	cache->setFunction(f);
};

QDiffCrossSection Kamae06::getDiffCrossSection(
		const QEnergy &E_proton,
		const QEnergy &E_gamma) const {
	if (cacheStoragePresent)
		return cache->getValue(E_proton, E_gamma);
	return getDiffCrossSectionDirectly(E_proton, E_gamma);
}

QDiffCrossSection Kamae06::getDiffCrossSectionDirectly(
		const QEnergy &E_proton,
		const QEnergy &E_gamma) const {

	PARAMSET params; /* struct where parameters are stored */
	//memset(&params, 0, sizeof(PARAMSET));

	/* proton kinetic energy in GeV */
	double T_proton_GeV = 
		static_cast<double>(E_proton/1_GeV);
	/* photon energ in GeV */
	double E_gamma_GeV = 
		static_cast<double>(E_gamma/1_GeV);

	gamma_param(T_proton_GeV, &params);
	double dsigma_dlogTp = sigma_incl_tot(
		    ID_GAMMA,
		    E_gamma_GeV,
		    std::min(T_proton_GeV, 1e5),
		    &params);

	return QDiffCrossSection(
		dsigma_dlogTp / E_gamma_GeV * (1_mbarn / 1_GeV));
}

} // namespace hermes 

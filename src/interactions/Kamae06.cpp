#include "hermes/interactions/Kamae06.h"

namespace hermes {

Kamae06::Kamae06() {

}

QDiffCrossSection Kamae06::getDiffCrossSection(
		const QEnergy &E_proton,
		const QEnergy &E_gamma) const {
	
	PARAMSET params; /* struct where parameters are stored */
	//memset(&params, 0, sizeof(PARAMSET));

	/* proton kinetic energy in GeV */
	double T_proton_GeV = 
		static_cast<double>(E_proton.convertTo(1_GeV));
	/* photon energ in GeV */
	double E_gamma_GeV = 
		static_cast<double>(E_gamma.convertTo(1_GeV));

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

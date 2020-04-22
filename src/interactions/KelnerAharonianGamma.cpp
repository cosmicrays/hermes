#include "hermes/interactions/KelnerAharonianGamma.h"

#include <cmath>

namespace hermes {
namespace interactions {

KelnerAharonianGamma::KelnerAharonianGamma() : DifferentialCrossSection() {}

QArea KelnerAharonianGamma::sigmaInelastic(const QEnergy &Tp) {
	constexpr QEnergy Tp_th =
	    (2.0 * m_pi0 + (m_pi0 * m_pi0) / (2.0 * m_proton)) * c_squared;

	double LX = std::log(static_cast<double>(Tp / Tp_th));
	double Threshold =
	    std::max(0., 1. - std::pow(static_cast<double>(Tp_th / Tp), 1.9));

	if (Tp < Tp_th)
		return 0 * mbarn;

	return (30.7 - 0.96 * LX + 0.18 * LX * LX) * std::pow(Threshold, 3.0) *
	       mbarn;
}

QDifferentialCrossSection
KelnerAharonianGamma::getDiffCrossSection(const QEnergy &E_proton,
					  const QEnergy &E_gamma) const {

	if (E_gamma > E_proton)
		return QDifferentialCrossSection(0);

	const double L = std::log(
	    static_cast<double>(E_proton / 1_TeV));	    // defined in pag. 9
	double x = static_cast<double>(E_gamma / E_proton); // defined in pag. 9

	double B_gamma = 1.30 + 0.14 * L + 0.011 * L * L;	    // Eq. 59
	double beta_gamma = 1. / (1.79 + 0.11 * L + 0.008 * L * L); // Eq. 60
	double k_gamma = 1. / (0.801 + 0.049 * L + 0.014 * L * L);  // Eq. 61
	double x2beta = std::pow(x, beta_gamma);

	double F_1 = (1. - x2beta) / (1. + k_gamma * x2beta * (1. - x2beta));
	double F_2 = 4. * beta_gamma * x2beta / (1. - x2beta);
	double F_3 = 4. * k_gamma * beta_gamma * x2beta * (1. - 2. * x2beta);
	F_3 /= 1. + k_gamma * x2beta * (1. - x2beta);

	double F_gamma = B_gamma * std::log(x) / x * std::pow(F_1, 4); // Eq. 58
	F_gamma *= 1. / log(x) - F_2 - F_3;

	return sigmaInelastic(E_proton) * F_gamma / E_proton;
}

} // namespace interactions
} // namespace hermes

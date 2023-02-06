#include "hermes/interactions/KelnerAharonianNeutrino.h"

#include <cmath>

namespace hermes { namespace interactions {

KelnerAharonianNeutrino::KelnerAharonianNeutrino() : DifferentialCrossSection() {}

QDiffCrossSection KelnerAharonianNeutrino::getDiffCrossSection(const QEnergy &E_proton, const QEnergy &E_nu) const {
	if (E_nu > E_proton) return QDiffCrossSection(0);

	const double L = std::log(static_cast<double>(E_proton / 1_TeV));  // defined in pag. 9

	const double B_e = 1.0 / (69.5 + 2.65 * L + 0.3 * L * L);                                // Eq. 63
	const double beta_e = 1. / std::pow(0.201 + 0.062 * L + 0.00042 * L * L, 0.25);          // Eq. 64
	const double k_e = (0.279 + 0.141 * L + 0.0172 * L * L) / (0.3 + std::pow(2.3 + L, 2));  // Eq. 65

	const double x = static_cast<double>(E_nu / E_proton);
	const double y = x / 0.427;

	double F_e = 0, F_numu = 0;

	if (L > -3.) {
		F_e = B_e * std::pow(1.0 + k_e * std::pow(std::log(x), 2), 3) * std::pow(-std::log(x), 5);
		F_e /= x * (1.0 + 0.3 / std::pow(x, beta_e));  // Eq. 62
	}

	if (y < 1) {
		double B_prime = 1.75 + 0.204 * L + 0.010 * L * L;             // Eq. 67
		double beta_prime = 1. / (1.67 + 0.111 * L + 0.0038 * L * L);  // Eq. 68
		double k_prime = 1.07 - 0.086 * L + 0.002 * L * L;             // Eq. 69
		double y2beta = std::pow(y, beta_prime);
		double F_1 = 4. * beta_prime * y2beta / (1. - y2beta);
		double F_2 = 4. * k_prime * beta_prime * y2beta * (1. - 2. * y2beta);
		F_2 /= 1. + k_prime * y2beta * (1. - y2beta);
		F_numu = B_prime * std::log(y) / y * std::pow((1. - y2beta) / (1. + k_prime * y2beta * (1. - y2beta)), 4);
		F_numu *= 1. / std::log(y) - F_1 - F_2;  // Eq. 66
	}

	return KelnerAharonianGamma::sigmaInelastic(E_proton) * (F_numu + 2. * F_e) / E_proton;
}

QDiffCrossSection KelnerAharonianNeutrino::getDiffCrossSection(const PID &projectile, const PID &target,
                                                               const QEnergy &E_proj,
                                                               const QEnergy &E_secondary) const {
	return nuclearScaling(projectile, target) * getDiffCrossSection(E_proj, E_secondary);
}

}}  // namespace hermes::interactions

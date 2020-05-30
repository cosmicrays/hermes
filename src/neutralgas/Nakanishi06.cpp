#include "hermes/neutralgas/Nakanishi06.h"

namespace hermes { namespace neutralgas {

Nakanishi06::Nakanishi06() : ProfileAbstract() {}
Nakanishi06::~Nakanishi06() {}

/* Ferriere2007, Nakanishi06, described in arXiv:1607.07886*/
QPDensity Nakanishi06::getPDensity(const Vector3QLength &pos) const {
	QLength h_0 = 1.06_pc;
	QPDensity n_0 = 0.94 / 1_cm3;
	double r_kpc = static_cast<double>(pos.getR() / 1_kpc);
	double exp_1 = std::exp(-r_kpc / 2.4);
	double exp_2 = std::exp(-std::pow((r_kpc - 9.5) / 4.8, 2));
	QLength scaleHeight = h_0 * (116.3 + 19.3 * r_kpc + 4.1 * std::pow(r_kpc,2) - 0.05 * std::pow(r_kpc, 3));
	QPDensity densityOnPlane = n_0 * (0.6 * exp_1 + 0.24 * exp_2);
	return densityOnPlane * exp(-M_LN2 * pow<2>(pos.z / scaleHeight));
}

}}  // namespace hermes::neutralgas

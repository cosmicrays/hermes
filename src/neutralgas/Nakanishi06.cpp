#include "hermes/neutralgas/Nakanishi06.h"

namespace hermes { namespace neutralgas {

Nakanishi06::Nakanishi06() : ProfileAbstract() {}
Nakanishi06::~Nakanishi06() {}

/* Nakanishi06, described in arXiv:1607.07886*/
QPDensity Nakanishi06::getHIDensity(const Vector3QLength &pos) const {
	const QLength h_0 = 1.06_pc;
	const QPDensity n_0 = 0.94 / 1_cm3;
	double r_kpc = static_cast<double>(pos.getR() / 1_kpc);
	double exp_1 = std::exp(-r_kpc / 2.4);
	double exp_2 = std::exp(-std::pow((r_kpc - 9.5) / 4.8, 2));
	QLength scaleHeight = h_0 * (116.3 + 19.3 * r_kpc + 4.1 * std::pow(r_kpc,2) - 0.05 * std::pow(r_kpc, 3));
	QPDensity densityOnPlane = n_0 * (0.6 * exp_1 + 0.24 * exp_2);
	return densityOnPlane * exp(-M_LN2 * pow<2>(pos.z / scaleHeight));
}

QPDensity Nakanishi06::getH2Density(const Vector3QLength &pos) const {
	double z_kpc = static_cast<double>(pos.z / 1_kpc);
	double r_kpc = static_cast<double>(pos.getR() / 1_kpc);
	double E1 = 11.2 * std::exp(-pow<2>(r_kpc) / 0.874);
	double E2 = 0.83 * std::exp(-pow<2>((r_kpc - 4.0) / 3.2));
	double h = 1.06e-3 * (10.8 * std::exp(0.28 * r_kpc) + 42.78);
	return 2.0 * 0.94 * (E1 + E2) * std::exp(-M_LN2 * pow<2>(z_kpc / h)) / 1_cm3;
}

QPDensity Nakanishi06::getPDensity(GasType gas, const Vector3QLength &pos) const {
    if (gas == GasType::HI) {
		return getHIDensity(pos);    
    }
	if (gas == GasType::H2) {
		return getH2Density(pos);    
	}
    return QPDensity(0);
}

}}  // namespace hermes::neutralgas

#include "hermes/cosmicrays/WMAP07CRDensity.h"

#include "hermes/Common.h"

namespace hermes { namespace cosmicrays {

WMAP07CRDensity::WMAP07CRDensity() : minE(1_GeV), maxE(1e4_GeV), steps(10) {
	makeEnergyRange();
	setParameters();
}

WMAP07CRDensity::WMAP07CRDensity(QEnergy minE_, QEnergy maxE_, int steps_)
    : minE(minE_), maxE(maxE_), steps(steps_) {
	makeEnergyRange();
	setParameters();
}

void WMAP07CRDensity::setParameters() {
	spectralIndex = 3;
	h_d = 1_kpc;
	h_r = 5_kpc;
	E_0 = 1_GeV;
	E_cutoff = 5_TeV;
	C_Earth = 6.4e-5 / 1_cm3;
	r_Earth = 8.5_kpc;
	C_0 = C_Earth * exp(r_Earth / h_r);
}

void WMAP07CRDensity::makeEnergyRange() {
	QEnergy energy = minE;
	double energyRatio =
	    exp(1. / static_cast<double>(steps - 1) * log(maxE / minE));

	for (int i = 0; i < steps; ++i) {
		energyRange.push_back(energy);
		energy = energy * energyRatio;
	}
}

QPDensityPerEnergy WMAP07CRDensity::getDensityPerEnergy(
    const QEnergy &E_, const Vector3QLength &pos_) const {
	QLength rho = sqrt(pos_.x * pos_.x + pos_.y * pos_.y);

	QNumber profile = exp(-rho / h_r) / (std::pow(cosh(pos_.z / h_d), 2));
	return C_0 * profile *
	       std::pow(static_cast<double>(getLorentzFactor(m_electron, E_)),
	                -spectralIndex) /
	       (m_electron * c_squared);
}

}}  // namespace hermes::cosmicrays

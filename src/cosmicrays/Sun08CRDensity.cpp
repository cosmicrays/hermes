#include "hermes/cosmicrays/Sun08CRDensity.h"
#include "hermes/Common.h"

namespace hermes { namespace cosmicrays {

Sun08CRDensity::Sun08CRDensity() : minE(1_GeV), maxE(1e4_GeV), steps(10) {
	makeEnergyRange();
	setParameters();
}

Sun08CRDensity::Sun08CRDensity(QEnergy minE_, QEnergy maxE_, int steps_) : minE(minE_), maxE(maxE_), steps(steps_) {
	makeEnergyRange();
	setParameters();
}

void Sun08CRDensity::setParameters() {
	spectralIndex = 3;
	h_d = 1_kpc;
	h_r = 8_kpc;
	C_0 = 6.4e-5 / 1_cm3;
	r_Earth = 8.5_kpc;
	//C_0 = C_Earth * exp(r_Earth/h_r);
}

void Sun08CRDensity::makeEnergyRange() {
	QEnergy energy = minE;
	float energyRatio =
		exp(1./static_cast<double>(steps-1) * log(maxE/minE));

	for (int i = 0; i < steps; ++i) {
		energyRange.push_back(energy);
		energy = energy * energyRatio;
	}
}

QPDensityPerEnergy Sun08CRDensity::getDensityPerEnergy(const QEnergy& E_, const Vector3QLength& pos_) const {
	
	if (fabs(pos_.z) > 1_kpc)
		return QPDensityPerEnergy(0);
	
	QLength rho = sqrt(pos_.x * pos_.x + pos_.y * pos_.y);

	if (rho < 3_kpc)
		rho = 3_kpc;

	QNumber profile = exp(-(rho - r_Earth) / h_r - fabs(pos_.z) / h_d);
	return C_0 * profile * std::pow(static_cast<double>(getLorentzFactor(m_electron, E_)), -spectralIndex) / (m_electron*c_squared);
}

} // namespace cosmicrays
} // namespace hermes

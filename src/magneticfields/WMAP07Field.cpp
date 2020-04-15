#include "hermes/magneticfields/WMAP07Field.h"
#include "hermes/Units.h"

namespace hermes { namespace magneticfields {

WMAP07Field::WMAP07Field() {
	// parameters from Waelkens et al. 2009 (Hammurabi)
	B_0 = 4.0_muG;
	r_0 = 8.0_kpc;
	z_0 = 1.0_kpc;
	chi_0 = 25.0_deg;
	psi_1 = 0.9_deg;
	psi_0 = 27.0_deg; // ~35.0_deg in the original paper
}

Vector3QMField WMAP07Field::getField(const Vector3QLength& pos_) const {
	Vector3QMField B(0);

	QLength rho = sqrt(pos_.x * pos_.x + pos_.y * pos_.y);
	QNumber cosphi = pos_.x / rho;
	QNumber sinphi = pos_.y / rho;

	if (rho < 3_kpc || rho > 20_kpc)
		return B; 

	QAngle psi = psi_0 + psi_1 * log(rho/r_0);
	QAngle chi = chi_0 * tanh(pos_.z/z_0);

	QNumber B_r = cos(psi)*cos(chi);
	QNumber B_phi = sin(psi)*cos(chi);

	B.x = B_0 * (B_phi * cosphi - B_r * sinphi);
	B.y = B_0 * (B_phi * sinphi + B_r * cosphi);
	B.z = B_0 * sin(chi);

	return B;
}

} // namespace magneticfields
} // namespace hermes

#include "hermes/magneticField/Sun08Field.h"
#include "hermes/Units.h"

namespace hermes {

Sun08Field::Sun08Field() {
	// regular field parameters
	R_Earth = 8.5_kpc;
	R_0 = 10_kpc;
	z_0 = 1_kpc;
	R_c = 5_kpc;
	B_0 = 2_muG;
	B_c = 2_muG;
	z_H0 = 1.5_kpc;
	z_H1 = 0.2_kpc;
	B_H0 = 10_muG;
	R_H0 = 4_kpc;
	pitch = 6_deg;

	// turbulent field parameters
	useTurbulent = false;
	B_turbulent = 3_muG;
}

#ifdef HERMES_HAVE_FFTW3F
void Sun08Field::randomTurbulent(int seed) {
	useTurbulent = true;
	// turbulent field with Kolmogorov spectrum, B_rms = 1 and Lc = 60 parsec
	turbulentGrid = std::make_shared<VectorGrid>(VectorGrid(Vector3d(0.), 256, static_cast<double>(4_pc)));
	initTurbulence(turbulentGrid, 1, static_cast<double>(8_pc), static_cast<double>(272_pc), -11./3., seed);
}
#endif

void Sun08Field::setTurbulentGrid(std::shared_ptr<VectorGrid> grid) {
	useTurbulent = true;
	turbulentGrid = grid;
}

void Sun08Field::setUseTurbulent(bool use) {
	if ((use) and (turbulentGrid)) {
		std::cout << "JF12Field: No turbulent field set: ignored" << std::endl;
		return;
	}
	useTurbulent = use;
}

bool Sun08Field::isUsingTurbulent() const {
	return useTurbulent;
}

Vector3QMField Sun08Field::getRegularField(const Vector3QLength& pos_) const {
	Vector3QMField B_disc(0);
	Vector3QMField B_halo(0);

	QLength rho = sqrt(pos_.x * pos_.x + pos_.y * pos_.y);
	QNumber cosphi = pos_.x / rho;
	QNumber sinphi = pos_.y / rho;

	// Disc field
	QMField D1(0);
	bool D2;
	
	D1 = (rho > R_c) ? B_0 * exp(-(rho - R_Earth)/R_0 - fabs(pos_.z)/z_0) : B_c;
	
	if (rho > 7.5_kpc) D2 = 1;
	if (6_kpc < rho && rho <= 7.5_kpc) D2 = -1;
	if (5_kpc < rho && rho <= 6_kpc) D2 = +1;
	if (rho <= 5_kpc) D2 = -1;

	QMField B_r = D1 * D2 * sin(pitch);
	QMField B_phi = -D1 * D2 * cos(pitch);
	QMField B_z = 0;

	B_disc.x = (B_phi * cosphi - B_r * sinphi);
	B_disc.y = (B_phi * sinphi + B_r * cosphi);
	B_disc.z = 0;

	// Halo field
	B_phi = B_H0 * 1 / (QNumber(1) + pow<2>((fabs(pos_.z) - z_H0)/z_H1)) * rho/R_H0 * exp(-(rho-R_H0)/R_H0);
	
	B_halo.x = (B_phi * cosphi - B_r * sinphi);
	B_halo.y = (B_phi * sinphi + B_r * cosphi);
	B_halo.z = 0;

	return B_disc + B_halo;
}

Vector3QMField Sun08Field::getTurbulentField(const Vector3QLength& pos_) const {
	return (turbulentGrid->interpolate(pos_) * B_turbulent);
}

Vector3QMField Sun08Field::getField(const Vector3QLength& pos_) const {
	Vector3QMField b(0.);

	b = getRegularField(pos_);
	if (useTurbulent)
		b += getTurbulentField(pos_);
	
	return b;
}

} // namespace hermes

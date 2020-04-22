#include "hermes/magneticfields/Sun08Field.h"
#include "hermes/Units.h"

namespace hermes {
namespace magneticfields {

Sun08Field::Sun08Field() {
    // regular field parameters
    R_Sun = 8.5_kpc;
    // disk
    pitch = -12.0_deg;
    disk_B_0 = 2_muG;
    disk_B_c = 2_muG;
    disk_z_0 = 1_kpc;
    disk_R_c = 5_kpc;
    disk_R_0 = 10_kpc;
    disk_R_1 = 7.5_kpc;
    disk_R_2 = 6.0_kpc;
    disk_R_3 = 5.0_kpc;
    // halo
    useHalo = true;
    halo_z_0 = 1.5_kpc;
    halo_z_1 = 0.2_kpc;
    halo_B_0 = 10_muG;
    halo_R_0 = 4_kpc;

    // turbulent field parameters
    useTurbulent = false;
    B_turbulent = 3_muG;
}

#ifdef HERMES_HAVE_FFTW3F
void Sun08Field::randomTurbulent(int seed) {
    useTurbulent = true;
    // turbulent field with Kolmogorov spectrum, B_rms = 1 and Lc = 60
    // parsec
    turbulentGrid = std::make_shared<VectorGrid>(
	VectorGrid(Vector3d(0.), 256, static_cast<double>(4_pc)));
    initTurbulence(turbulentGrid, 1, static_cast<double>(8_pc),
		   static_cast<double>(272_pc), -11. / 3., seed);
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

void Sun08Field::setUseHalo(bool use) { useHalo = use; }

bool Sun08Field::isUsingTurbulent() const { return useTurbulent; }

bool Sun08Field::isUsingHalo() const { return useHalo; }

Vector3QMField Sun08Field::getRegularField(const Vector3QLength &pos_) const {
    Vector3QMField B_disk(0);
    Vector3QMField B_halo(0);

    QLength rho = sqrt(pos_.x * pos_.x + pos_.y * pos_.y);
    QNumber cosphi = pos_.x / rho;
    QNumber sinphi = pos_.y / rho;

    // Disk field
    QMField D1(0);
    int D2 = 1;

    D1 = (rho > disk_R_c) ? disk_B_0 * exp(-(rho - R_Sun) / disk_R_0 -
					   fabs(pos_.z) / disk_z_0)
			  : disk_B_c;

    if (rho > disk_R_1)
	D2 = 1;
    if (rho > disk_R_2 && rho <= disk_R_1)
	D2 = -1;
    if (rho > disk_R_3 && rho <= disk_R_2)
	D2 = +1;
    if (rho <= disk_R_3)
	D2 = -1;

    QMField B_r = D1 * D2 * sin(pitch);
    QMField B_phi = -D1 * D2 * cos(pitch);
    QMField B_z = 0;

    B_disk.x = (B_r * cosphi - B_phi * sinphi);
    B_disk.y = (-B_r * sinphi - B_phi * cosphi);
    B_disk.z = B_z;

    // Halo field
    if (useHalo) {
	B_r = 0_muG;
	B_phi = halo_B_0 * 1 /
		(QNumber(1) + pow<2>((fabs(pos_.z) - halo_z_0) / halo_z_1)) *
		rho / halo_R_0 * exp(-(rho - halo_R_0) / halo_R_0);

	B_halo.x = (B_r * cosphi - B_phi * sinphi);
	B_halo.y = (-B_r * sinphi - B_phi * cosphi);
	B_halo.z = 0;
    }

    return B_disk + B_halo;
}

Vector3QMField Sun08Field::getTurbulentField(const Vector3QLength &pos_) const {
    return (turbulentGrid->interpolate(pos_) * B_turbulent);
}

Vector3QMField Sun08Field::getField(const Vector3QLength &pos_) const {
    Vector3QMField b(0.);

    b = getRegularField(pos_);
    if (useTurbulent)
	b += getTurbulentField(pos_);

    return b;
}

} // namespace magneticfields
} // namespace hermes

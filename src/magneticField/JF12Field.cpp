#include "hermes/magneticField/JF12Field.h"
#include "hermes/GridTools.h"
#include "hermes/Random.h"

#include <iostream>

namespace hermes {

double logisticFunction(QLength x, QLength x0, QLength w) {
	return 1. / (1. + exp(-2. * (fabs(x) - x0) / w));
}

JF12Field::JF12Field() {
	useRegular = true;
	useStriated = false;
	useTurbulent = false;

	// spiral arm parameters
	pitch = 11.5 * M_PI / 180;
	sinPitch = sin(pitch);
	cosPitch = cos(pitch);
	tan90MinusPitch = tan(1_pi * radian / 2 - pitch);

	rArms[0] = 5.1_kpc;
	rArms[1] = 6.3_kpc;
	rArms[2] = 7.1_kpc;
	rArms[3] = 8.3_kpc;
	rArms[4] = 9.8_kpc;
	rArms[5] = 11.4_kpc;
	rArms[6] = 12.7_kpc;
	rArms[7] = 15.5_kpc;

	// regular field parameters
	bRing = 0.1_muG;
	hDisk = 0.40_kpc;
	wDisk = 0.27_kpc;

	bDisk[0] = 0.1_muG;
	bDisk[1] = 3.0_muG;
	bDisk[2] = -0.9_muG;
	bDisk[3] = -0.8_muG;
	bDisk[4] = -2.0_muG;
	bDisk[5] = -4.2_muG;
	bDisk[6] = 0.0_muG;
	bDisk[7] = 2.7_muG;

	bNorth = 1.4_muG;
	bSouth = -1.1_muG;
	rNorth = 9.22_kpc;
	rSouth = 17.0_kpc;
	wHalo = 0.20_kpc;
	z0 = 5.3_kpc;

	bX = 4.6_muG;
	thetaX0 = 49.0 * M_PI / 180;
	sinThetaX0 = sin(thetaX0);
	cosThetaX0 = cos(thetaX0);
	tanThetaX0 = tan(thetaX0);
	rXc = 4.8_kpc;
	rX = 2.9_kpc;

	// striated field parameter
	sqrtbeta = std::sqrt(1.36);

	// turbulent field parameters
	bDiskTurb[0] = 10.81_muG;
	bDiskTurb[1] = 6.96_muG;
	bDiskTurb[2] = 9.59_muG;
	bDiskTurb[3] = 6.96_muG;
	bDiskTurb[4] = 1.96_muG;
	bDiskTurb[5] = 16.34_muG;
	bDiskTurb[6] = 37.29_muG;
	bDiskTurb[7] = 10.35_muG;

	bDiskTurb5 = 7.63_muG;
	zDiskTurb = 0.61_kpc;

	bHaloTurb = 4.68_muG;
	rHaloTurb = 10.97_kpc;
	zHaloTurb = 2.84_kpc;
}

void JF12Field::randomStriated(int seed) {
	useStriated = true;
	int N = 100;
	striatedGrid = std::make_shared<ScalarGrid>(Vector3d(0.), N, (0.1_kpc).getValue());

	Random random;
	if (seed != 0)
		random.seed(seed);

	for (int ix = 0; ix < N; ix++)
		for (int iy = 0; iy < N; iy++)
			for (int iz = 0; iz < N; iz++) {
				float &f = striatedGrid->get(ix, iy, iz);
				f = round(random.rand()) * 2 - 1;
			}
}

#ifdef HERMES_HAVE_FFTW3F
void JF12Field::randomTurbulent(int seed) {
	useTurbulent = true;
	// turbulent field with Kolmogorov spectrum, B_rms = 1 and Lc = 60 parsec
	turbulentGrid = new VectorGrid(Vector3d(0.), 256, 4 * parsec);
	initTurbulence(turbulentGrid, 1, 8 * parsec, 272 * parsec, -11./3., seed);
}
#endif

void JF12Field::setStriatedGrid(std::shared_ptr<ScalarGrid> grid) {
	useStriated = true;
	striatedGrid = grid;
}

void JF12Field::setTurbulentGrid(std::shared_ptr<VectorGrid> grid) {
	useTurbulent = true;
	turbulentGrid = grid;
}

std::shared_ptr<ScalarGrid> JF12Field::getStriatedGrid() {
	return striatedGrid;
}

std::shared_ptr<VectorGrid> JF12Field::getTurbulentGrid() {
	return turbulentGrid;
}

void JF12Field::setUseRegular(bool use) {
	useRegular = use;
}

void JF12Field::setUseStriated(bool use) {
	if ((use) and (striatedGrid)) {
		std::cout << "JF12Field: No striated field set: ignored" << std::endl;
		return;
	}
	useStriated = use;
}

void JF12Field::setUseTurbulent(bool use) {
	if ((use) and (turbulentGrid)) {
		std::cout << "JF12Field: No turbulent field set: ignored" << std::endl;
		return;
	}
	useTurbulent = use;
}

bool JF12Field::isUsingRegular() {
	return useRegular;
}

bool JF12Field::isUsingStriated() {
	return useStriated;
}

bool JF12Field::isUsingTurbulent() {
	return useTurbulent;
}

Vector3QMField JF12Field::getRegularField(const Vector3QLength& pos) const {
	Vector3QMField b(0.);

	QLength r = sqrt(pos.x * pos.x + pos.y * pos.y); // in-plane radius
	QLength d = pos.getR(); // distance to galactic center
	if ((d < 1.0_kpc) or (d > 20.0_kpc))
		return b; // 0 field for d < 1 kpc or d > 20 kpc

	QAngle phi = pos.getPhi(); // azimuth
	double sinPhi = sin(phi);
	double cosPhi = cos(phi);

	double lfDisk = logisticFunction(pos.z, hDisk, wDisk);

	// disk field
	if (r > 3_kpc) {
		QMField bMag;
		if (r < 5_kpc) {
			// molecular ring
			bMag = (1 - lfDisk) * bRing * (5_kpc / r);
			b.x += sinPhi * -bMag;
			b.y += cosPhi * bMag;

		} else {
			// spiral region
			QLength r_negx = r * exp(-(phi - pi * radian).getValue() / tan90MinusPitch);
			if (r_negx > rArms[7])
				r_negx = r * exp(-(phi + pi * radian).getValue() / tan90MinusPitch);
			if (r_negx > rArms[7])
				r_negx = r * exp(-(phi + 3 * pi * radian).getValue() / tan90MinusPitch);

			for (int i = 7; i >= 0; i--)
				if (r_negx < rArms[i])
					bMag = bDisk[i];

			bMag *= (5_kpc / r) * (1 - lfDisk);
			b.x += bMag * (sinPitch * cosPhi - cosPitch * sinPhi);
			b.y += bMag * (sinPitch * sinPhi + cosPitch * cosPhi);
		}
	}

	// toroidal halo field
	QMField bMagH = exp(-fabs(pos.z) / z0) * lfDisk;
	if (pos.z >= 0_kpc)
		bMagH *= bNorth * (1 - logisticFunction(r, rNorth, wHalo));
	else
		bMagH *= bSouth * (1 - logisticFunction(r, rSouth, wHalo));
	b.x += -bMagH * sinPhi;
	b.y += bMagH * cosPhi;

	// poloidal halo field
	QMField bMagX;
	double sinThetaX, cosThetaX;
	QLength rp;
	QLength rc = rXc + fabs(pos.z) / tanThetaX0;
	if (r < rc) {
		// varying elevation region
		rp = r * rXc / rc;
		bMagX = bX * exp(-1 * rp / rX) * (rXc / rc) * (rXc / rc);
		QAngle thetaX = atan2(fabs(pos.z), (r - rp));
		if (pos.z == 0_kpc)
			thetaX = M_PI / 2.;
		sinThetaX = sin(thetaX);
		cosThetaX = cos(thetaX);
	} else {
		// constant elevation region
		rp = r - fabs(pos.z) / tanThetaX0;
		bMagX = bX * exp(-rp / rX) * (rp / r);
		sinThetaX = sinThetaX0;
		cosThetaX = cosThetaX0;
	}
	double zsign = pos.z < 0_kpc ? -1 : 1;
	b.x += zsign * bMagX * cosThetaX * cosPhi;
	b.y += zsign * bMagX * cosThetaX * sinPhi;
	b.z += bMagX * sinThetaX;

	return b;
}

Vector3QMField JF12Field::getStriatedField(const Vector3QLength& pos) const {
	return (getRegularField(pos)
			* (1. + sqrtbeta * striatedGrid->closestValue(
			 		pos
			  	)
			  )
	);
}

QMField JF12Field::getTurbulentStrength(const Vector3QLength& pos) const {
	if (pos.getR() > 20.0_kpc)
		return 0_muG;

	QLength r = sqrt(pos.x * pos.x + pos.y * pos.y); // in-plane radius
	QAngle phi = pos.getPhi(); // azimuth

	// disk
	QMField bDisk = 0_muG;
	if (r < 5.0_kpc) {
		bDisk = bDiskTurb5;
	} else {
		// spiral region
		QLength r_negx = r * exp(-(phi - pi*radian).getValue() / tan90MinusPitch);
		if (r_negx > rArms[7])
			r_negx = r * exp(-(phi + pi*radian).getValue() / tan90MinusPitch);
		if (r_negx > rArms[7])
			r_negx = r * exp(-(phi + 3 * pi*radian).getValue() / tan90MinusPitch);

		for (int i = 7; i >= 0; i--)
			if (r_negx < rArms[i])
				bDisk = bDiskTurb[i];

		bDisk *= (5.0_kpc) / r;
	}
	bDisk *= exp(-0.5 * (pos.z / zDiskTurb) * (pos.z / zDiskTurb));

	// halo
	QMField bHalo = bHaloTurb * exp(-r / rHaloTurb)
			* exp(-0.5 * (pos.z / zHaloTurb) * (pos.z / zHaloTurb));

	// modulate turbulent field
	return sqrt(bDisk*bDisk + bHalo*bHalo);
}

Vector3QMField JF12Field::getTurbulentField(const Vector3QLength& pos) const {
	return (turbulentGrid->interpolate(pos) * getTurbulentStrength(pos));
}

Vector3QMField JF12Field::getField(const Vector3QLength& pos) const {
	Vector3QMField b(0.);
	if (useTurbulent)
		b += getTurbulentField(pos);
	if (useStriated)
		b += getStriatedField(pos);
	else if (useRegular)
		b += getRegularField(pos);
	return b;
}

} // namespace hermes

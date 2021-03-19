#include "hermes/ionizedgas/NE2001Simple.h"

#include <iostream>

namespace hermes { namespace ionizedgas {

NE2001Simple::NE2001Simple() {
	setTemperature(1e4_K);

	/*
	gal01.inp = input parameters for large-scale components of NE2001 30
	June '02 1 1 1 1 1 1 1 wg1 wg2 wga wggc wglism wgcN, wgvN wa:     0.65
	Fa:     5 narm1:  0.5 narm2:  1.2 narm3:  1.3 narm4:  1.0 narm5:  0.25
	warm1:  1.0
	warm2:  1.5
	warm3:  1.0
	warm4:  0.8
	warm5:  1.0
	harm1:  1.0
	harm2:  0.8
	harm3:  1.3
	harm4:  1.5
	harm5:  1.0
	farm1:  1.1
	farm2:  0.3
	farm3:  0.4
	farm4:  1.5
	farm5:  0.3
	*/
	N_arms = 5;

	H1 = 0.97_kpc;  // paper: 0.95_kpc
	H2 = 0.15_kpc;  // paper: 0.14_kpc
	H_a = 0.23_kpc;
	n1 = 0.033_kpc / 1_cm3 / H1;  // given as n1*H1 (cm^-3 kpc)
	n2 = 0.08 / 1_cm3;
	n_a = 0.028 / 1_cm3;
	A1 = 17.5_kpc;
	A2 = 3.8_kpc;
	A_a = 10.5_kpc;
	F1 = 0.18;
	F2 = 120;
	R_Sun = 8.5_kpc;
	/*
	Fgc0 = 0.6e5
	*/
	x_GC = -0.01_kpc;
	y_GC = 0.0_kpc;
	z_GC = -0.020_kpc;
	r_GC = 0.145_kpc;
	H_GC = 0.026_kpc;
	n_GC = 10.0 / 1_cm3;

	/*
	  ne_arms_log.inp
	   a    rmin  thmin  extent
	  4.25  3.48  0.      6.
	  4.25  3.48  3.141   6.
	  4.89  4.90  2.525   6.
	  4.89  3.76  4.24    6.
	  4.57  8.10  5.847   0.55
	*/
}

/* Table 2 NE2001 Model */
/*
    // Total el. density
    n_e(x) = (1 - w_voids) * (
            (1-w_lism)*(n_gal(x) + n_GC(x)
        )
        + w_lism*n_lism(x))
        + w_voids*n_voids(x)
        + n_clumps(x)

    // Smooth components
    n_gal(x) = n_1*G_1(r,z)
           + n_2*G_2(r,z)
           + n_alpha*G_alpha(x)

    n_1*G_1(r,z) = n_1*g_1(r)*h(z/H_1); // Thick disk
    n_2*G_2(r,z) = n_2*g_2(r)*h(z/H_2); // Thin disk
    n_alpha * G_alpha(x); // Spiral arms

    // Galactic centre, n_GC
    n_GC(x) = n_GC0 * exp(-((dr/R_GC)^2 + (z-z_GC)^2/H_GC^2))

    // Local ISM, n_lism
    n_lism(x), F_lism(x), w_lism(x)

    // Clumps, n_clumps

    // Voids, n_voids

    // Functions
    h(x) = sech(x)^2
    U(x) = unit step function
    g1(r) = [cos(pi*r/(2*A1))/cos(pi*R_o/(2*A1))]*U(r-A1)
    g2(r) = exp(-(r-A_a)^2/A_a^2)*U(r)
    Ga(x) = sum_j ( f_j* ga_j )
*/

double NE2001Simple::h(const QNumber &x) const {
	// return std::pow(2.0/(exp(x)+exp(-x)), 2);
	double cosh_ = cosh(x);
	return 1.0 / (cosh_ * cosh_);
}

QPDensity NE2001Simple::getDensity(const Vector3QLength &pos) const {
	QPDensity total(0);
	total += getThickDiskDensity(pos);
	total += getThickDiskDensity(pos);
	total += getSpiralArmsDensity(pos);
	total += getGalacticCentreDensity(pos);

	return total;
}

QPDensity NE2001Simple::getThickDiskDensity(const Vector3QLength &pos) const {
	QLength r = pos.getR();
	QNumber g1 = cos(pi / 2 * 1_rad * r / A1) / cos(pi / 2 * 1_rad * R_Sun / A1) * stepFunction(A1 - r);
	return n1 * g1 * h(pos.z / H1);
}

QPDensity NE2001Simple::getThinDiskDensity(const Vector3QLength &pos) const {
	QLength r = pos.getR();
	// QNumber g2 = exp(-(r-A2)*(r-A2)/(A2*A2)) * stepFunction(r); // paper
	QNumber g2 = exp(-(r - A2) * (r - A2) / (1.8_kpc * 1.8_kpc)) *
	             stepFunction(10_kpc - r);  // original code
	// I
	return n2 * g2 * h(pos.z / H2);
}

QPDensity NE2001Simple::getSpiralArmsDensity(const Vector3QLength &pos) const {
	QLength r = pos.getR();

	// Spiral arms
	QNumber G_a(0);
	for (int j = 0; j < N_arms; ++j) {
		// G_a += f[j] * g_a[j](r, s_j/w_j) * h(z/(h[j]*h_a));
	}

	return n_a * G_a;
}

QPDensity NE2001Simple::getGalacticCentreDensity(const Vector3QLength &pos) const {
	QLength r = pos.getR();
	QLength dr = sqrt((pos.x - x_GC) * (pos.x - x_GC) + (pos.y - y_GC) * (pos.y - y_GC));
	QLength dz = fabs(pos.z - z_GC);

	if (dr > r_GC || dz > H_GC) return QPDensity(0);

	// QNumber g_GC = exp(-(dr*dr/(r_GC*r_GC) + dz*dz/(H_GC*H_GC))); //
	// paper
	QNumber g_GC = (dr * dr / (r_GC * r_GC) + dz * dz / (H_GC * H_GC));  // original code

	// return n_GC * g_GC; // paper
	if (g_GC < QNumber(1))  // original paper
		return n_GC;
	return QPDensity(0);
}
/*
std::pair<std::vector<QLength>, std::vector<QLength>>
NE2001Simple::csplineInterpolation( const std::pair<std::vector<QLength>,
std::vector<QLength>> xy, int n) const {

    xout, yout

    if (nn > nmax)
        return; // too many points to spline

    n = abs(nn);
    if (nn < 0) {
        y2[0] = 0.;
        u[0] = 0.;
        for (int i = 2; i <= n-1; ++i) {
            double sig = (x[i] - x[i-1]) / (x[i+1] - x[i-1]);
            double p = sig * y2[i-2] + 2.;
            y2[i-1] = (sig - 1.) / p;
            u[i-1] = (((y[i+1] - y[i]) / (x[i+1] - x[i]) -
                (y[i] - y[i-1]) / (x[i] - x[i-1]))
                * 6. / (x[i+1] - x[i-1]) - sig * u[i-2])/p;
        }
        double qn = 0.;
        double un = 0.;
        y2[n-1] = (un - qn * u[n-2]) / (qn * y2[n-2] + 1.);
        for (int k = n-1; k >= 1; --k) {
            y2[k-1] = y2[k-1] * y2[k] + u[k-1];
        }
    }
    int klo = 1;
    int khi = n;
    while (khi-klo > 1) {
        k = (khi + klo) / 2;
        if (x[k] > *xout) {
            khi = k;
        } else {
            kl0 = k;
        }
    }
    double h = x[khi] - x[klo];
    double a = (x[khi] - *xout) / h;
    double b = (*xout - x[klo]) / h;

    *yout = a * y[klo] + b * y[khi] + ((r__1 * (r__1 * r__1) - a) * y2[klo -
        1] + (r__2 * (r__2 * r__2) - b) * y2[khi - 1]) * (r__3 * r__3) /
        6.f;

    return 0;
}
*/

}}  // namespace hermes::ionizedgas

#include "hermes/HEALPixBits.h"
#include <cmath>

#include <iostream>

namespace hermes {

unsigned int nside2npix(unsigned int nside) { return 12 * nside * nside; }

/*! Returns the remainder of the division \a v1/v2.
    The result is non-negative.
    \a v1 can be positive or negative; \a v2 must be positive. */
inline double fmodulo(double v1, double v2) {
	if (v1 >= 0)
		return (v1 < v2) ? v1 : std::fmod(v1, v2);
	double tmp = std::fmod(v1, v2) + v2;
	return (tmp == v2) ? 0. : tmp;
}

unsigned int log2(unsigned int x) {
	unsigned int res = 0;
	while (x >>= 1)
		res++;
	return res;
}

inline long nside2order(unsigned int nside) {
	return ((nside) & (nside - 1)) ? -1 : log2(nside);
}

unsigned int ang2pix_ring(unsigned int nside, const QDirection &thetaphi) {
	double theta = static_cast<double>(thetaphi[0]);
	double phi = static_cast<double>(thetaphi[1]);

	return ((theta < 0.01) || (theta > 3.14159 - 0.01))
		   ? loc2pix(nside, cos(theta), phi, sin(theta), true)
		   : loc2pix(nside, cos(theta), phi, 0., false);
}

QDirection pix2ang_ring(unsigned int nside, unsigned int ipix) {
	QDirection thetaphi = {0, 0};

	// parameter (ns_max = 8192) ! 2^13 : largest nside available
	const unsigned int npix = nside2npix(nside); // ! total number of points

	const unsigned int ipix1 = ipix + 1; // in {1, npix}
	const unsigned int nl2 = 2 * nside;
	const unsigned int nl4 = 4 * nside;
	const unsigned int ncap =
	    2 * nside *
	    (nside - 1); // ! points in each polar cap, =0 for nside =1
	const double fact1 = 1.5 * nside;
	const double fact2 = 3.0 * nside * nside;

	/* check in : src/C/subs/chealpix.c */

	// ! North Polar cap
	if (ipix1 <= ncap) {
		const double hip = ipix1 / 2.;
		const unsigned int fihip = std::floor(hip);
		const unsigned int iring =
		    (unsigned)std::floor(std::sqrt(hip - std::sqrt(fihip))) +
		    1; // ! counted from North pole
		const unsigned int iphi = ipix1 - 2 * iring * ((int)iring - 1);

		thetaphi[0] = std::acos(1. - iring * iring / fact2);
		thetaphi[1] = (1. * iphi - 0.5) * pi / (2. * iring);

		return thetaphi;
	}

	// ! Equatorial region
	if (ipix1 <= nl2 * (5 * nside + 1)) {
		const unsigned ip = ipix1 - ncap - 1;
		const unsigned int iring = (unsigned)std::floor(ip / nl4) +
					   nside; // ! counted from North pole
		const unsigned int iphi = (unsigned)std::fmod(ip, nl4) + 1;

		const double fodd =
		    0.5 *
		    (1 +
		     std::fmod((double)(iring + nside),
			       2)); //  ! 1 if iring+nside is odd, 1/2 otherwise

		thetaphi[0] = std::acos(((int)nl2 - (int)iring) / fact1);
		thetaphi[1] = (1. * iphi - fodd) * pi / (2. * nside);

		return thetaphi;
	}

	//! South Polar cap
	{
		const unsigned ip = npix - ipix1 + 1;
		const double hip = ip / 2.;
		const unsigned int fihip = std::floor(hip);
		const unsigned int iring =
		    (int)std::floor(std::sqrt(hip - std::sqrt(fihip))) +
		    1; //     ! counted from South pole
		const unsigned iphi =
		    (int)(4. * iring + 1 -
			  ((int)ip - 2. * (int)iring * ((int)iring - 1)));

		thetaphi[0] = std::acos(-1. + (int)iring * iring / fact2);
		thetaphi[1] = (1. * iphi - 0.5) * pi / (2. * iring);

		return thetaphi;
	}
}

unsigned int loc2pix(unsigned int nside, double z, double phi, double sth,
		     bool have_sth) {

	const double twothird = 2. / 3.;
	const double inv_halfpi = 1. / (pi / 2.);

	const long order = nside2order(nside);
	const unsigned int ncap =
	    2 * nside *
	    (nside - 1); // ! points in each polar cap, =0 for nside =1
	const double za = std::abs(z);
	const double tt = fmodulo(phi * inv_halfpi, 4.0); // in [0,4)

	if (za <= twothird) // Equatorial region
	{
		const unsigned int nl4 = 4 * nside;
		const double temp1 = nside * (0.5 + tt);
		const double temp2 = nside * z * 0.75;
		const long jp =
		    std::floor(temp1 - temp2); // index of  ascending edge line
		const long jm =
		    std::floor(temp1 + temp2); // index of descending edge line

		// ring number counted from z=2/3
		const long ir = nside + 1 + jp - jm; // in {1,2n+1}
		const long kshift =
		    1 - (ir & 1); // kshift=1 if ir even, 0 otherwise

		const long t1 = jp + jm - nside + kshift + 1 + nl4 + nl4;
		const long ip = (order > 0) ? (t1 >> 1) & (nl4 - 1)
					    : ((t1 >> 1) % nl4); // in {0,4n-1}

		return ncap + (ir - 1) * nl4 + ip;
	}

	// North & South polar caps
	{
		const double tp = tt - std::floor(tt);
		const double tmp =
		    ((za < 0.99) || (!have_sth))
			? nside * std::sqrt(3 * (1 - za))
			: nside * sth / std::sqrt((1. + za) / 3.);

		const long jp =
		    std::floor(tp * tmp); // increasing edge line index
		const long jm =
		    std::floor((1.0 - tp) * tmp); // decreasing edge line index

		const long ir =
		    jp + jm + 1; // ring number counted from the closest pole
		const long ip = std::floor((double)ir * tt); // in {0,4*ir-1}

		return (z > 0) ? 2 * ir * (ir - 1) + ip
			       : nside2npix(nside) - 2 * ir * (ir + 1) + ip;
	}
}

} // namespace hermes

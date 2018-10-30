#include "hermes/HEALPix.h"

namespace hermes {

inline long nside2npix(long nside) {
	return 12*nside*nside;
}

/*! Returns the remainder of the division \a v1/v2.
    The result is non-negative.
    \a v1 can be positive or negative; \a v2 must be positive. */
inline double fmodulo (double v1, double v2) {
	if (v1>=0)
		return (v1<v2) ? v1 : std::fmod(v1,v2);
	double tmp=std::fmod(v1,v2)+v2;
		return (tmp==v2) ? 0. : tmp;
}

std::size_t log2(std::size_t x) {
                std::size_t res = 0;
                while (x >>= 1) res++;
                return res;
}

inline long nside2order(long nside) {
	return ((nside)&(nside-1)) ? -1 : log2(nside);
}

std::array<QAngle, 2> pix2ang_ring(long nside, long ipix) {
	std::array<QAngle, 2> thetaphi = {0, 0};

	int nl2, nl4, npix, ncap, iring, iphi, ip, ipix1;
	double  fact1, fact2, fodd, hip, fihip;
	double PI=M_PI;

	/* check in : src/C/subs/chealpix.c */

	//      parameter (ns_max = 8192) ! 2^13 : largest nside available  
	npix = nside2npix(nside);      // ! total number of points

	ipix1 = ipix + 1; // in {1, npix}
	nl2 = 2*nside;
	nl4 = 4*nside;
	ncap = 2*nside*(nside-1);// ! points in each polar cap, =0 for nside =1
	fact1 = 1.5*nside;
	fact2 = 3.0*nside*nside;

	if( ipix1 <= ncap ) {  //! North Polar cap -------------
		hip   = ipix1/2.;
		fihip = floor(hip);
		iring = (int)floor( std::sqrt( hip - std::sqrt(fihip) ) ) + 1;// ! counted from North pole
		iphi  = ipix1 - 2*iring*(iring - 1);

		thetaphi[0] = acos( 1. - iring*iring / fact2 );
		thetaphi[1] = (1.*iphi - 0.5) * PI/(2.*iring);
	} else if( ipix1 <= nl2*(5*nside+1) ) {//then ! Equatorial region ------

		ip    = ipix1 - ncap - 1;
		iring = (int)floor( ip / nl4 ) + nside;// ! counted from North pole
		iphi  = (int)fmod(ip,nl4) + 1;

		fodd  = 0.5 * (1 + fmod((double)(iring+nside),2));//  ! 1 if iring+nside is odd, 1/2 otherwise
		thetaphi[0] = acos( (nl2 - iring) / fact1 );
		thetaphi[1]   = (1.*iphi - fodd) * PI /(2.*nside);
	} else {//! South Polar cap -----------------------------------

		ip    = npix - ipix1 + 1;
		hip   = ip/2.;
		/* bug corrige floor instead of 1.* */
		fihip = floor(hip);
		iring = (int)floor( std::sqrt( hip - std::sqrt(fihip) ) ) + 1;//     ! counted from South pole
		iphi  = (int)(4.*iring + 1 - (ip - 2.*iring*(iring-1)));

		thetaphi[0]   = acos( -1. + iring*iring / fact2 );
		thetaphi[1]   = (1.*iphi - 0.5) * PI/(2.*iring);
	}

	return thetaphi;
}


long ang2pix_ring(long nside, std::array<QAngle,2> thetaphi) {
	double theta = thetaphi[0].getValue();
	double phi = thetaphi[1].getValue();

	return ((theta<0.01) || (theta > 3.14159-0.01)) ?
        	loc2pix(nside, cos(theta),phi,sin(theta),true) :
	        loc2pix(nside, cos(theta),phi,0.,false);
}

long loc2pix(long nside, double z, double phi, double sth, bool have_sth) {

	const double twothird = 2./3.;
	const double inv_halfpi = 1./(M_PI/2.);

	long order = nside2order(nside);
	long ncap = 2*nside*(nside-1);// ! points in each polar cap, =0 for nside =1
	double za = std::abs(z);
	double tt = fmodulo(phi*inv_halfpi,4.0); // in [0,4)

	if (za<=twothird) // Equatorial region
	{
		long nl4 = 4*nside;
		double temp1 = nside*(0.5+tt);
		double temp2 = nside*z*0.75;
		long jp = long(temp1-temp2); // index of  ascending edge line
		long jm = long(temp1+temp2); // index of descending edge line

		// ring number counted from z=2/3
		long ir = nside + 1 + jp - jm; // in {1,2n+1}
		long kshift = 1-(ir&1); // kshift=1 if ir even, 0 otherwise

		long t1 = jp+jm-nside+kshift+1+nl4+nl4;
		long ip = (order>0) ?
			(t1>>1)&(nl4-1) : ((t1>>1)%nl4); // in {0,4n-1}

		return ncap + (ir-1)*nl4 + ip;
	}
	else  // North & South polar caps
	{
		double tp = tt-long(tt);
		double tmp = ((za<0.99)||(!have_sth)) ?
			nside*std::sqrt(3*(1-za)) :
			nside*sth/std::sqrt((1.+za)/3.);

		long jp = long(tp*tmp); // increasing edge line index
		long jm = long((1.0-tp)*tmp); // decreasing edge line index

		long ir = jp+jm+1; // ring number counted from the closest pole
		long ip = long(tt*ir); // in {0,4*ir-1}
		//planck_assert((ip>=0)&&(ip<4*ir),"must not happen");
		//ip %= 4*ir;

		return (z>0) ? 2*ir*(ir-1) + ip : nside2npix(nside) - 2*ir*(ir+1) + ip;
	}
}

} // namespace hermes 

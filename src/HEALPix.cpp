#include "hermes/HEALPix.h"

namespace hermes {

std::array<QAngle, 2> pix2ang_ring(long nside, long ipix) {
	std::array<QAngle, 2> thetaphi = {0, 0};

	int nl2, nl4, npix, ncap, iring, iphi, ip, ipix1;
	double  fact1, fact2, fodd, hip, fihip;
	double PI=M_PI;

	/* check in : src/C/subs/chealpix.c */

	//      parameter (ns_max = 8192) ! 2^13 : largest nside available  
	npix = 12*nside*nside;      // ! total number of points

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

} // namespace hermes 

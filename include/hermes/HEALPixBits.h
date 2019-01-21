#ifndef HERMES_HEALPIXBITS_H
#define HERMES_HEALPIXBITS_H

#include "hermes/Units.h"

#include <array>
#include <cmath>

namespace hermes {

std::size_t log2(std::size_t x);

// Adopted from HEALPix pix2ang_ring (https://healpix.sourceforge.io/)
long nside2order(long nside);
long nside2npix(long nside);
std::array<QAngle, 2> pix2ang_ring(long nside, long ipix); 
long ang2pix_ring(long nside, std::array<QAngle,2> thetaphi);
long loc2pix(long _nside, double z, double phi, double sth, bool have_sth);

} // namespace hermes

#endif // HERMES_HEALPIXBITS_H

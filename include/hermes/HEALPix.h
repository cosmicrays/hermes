#ifndef HERMES_HEALPIX_H
#define HERMES_HEALPIX_H

#include "hermes/Units.h"

#include <array>
#include <cmath>

namespace hermes {

// Adopted from HEALPix pix2ang_ring (https://healpix.sourceforge.io/)
std::array<QAngle, 2> pix2ang_ring(long nside, long ipix); 


} // namespace hermes

#endif // HERMES_HEALPIX_H

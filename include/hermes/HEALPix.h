#include "hermes.h"

#include <array>
#include <cmath>

namespace hermes {

// Adopted from HEALPix pix2ang_ring (https://healpix.sourceforge.io/)
std::array<QAngle, 2> pix2ang_ring(const long nside, const long ipix); 


} // namespace hermes

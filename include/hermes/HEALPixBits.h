#ifndef HERMES_HEALPIXBITS_H
#define HERMES_HEALPIXBITS_H

#include <cstddef>

#include "hermes/Units.h"

namespace hermes {

using namespace units;

static const double UNSEEN = -1.6375e30;

unsigned int log2(unsigned int x);

// Adopted from HEALPix pix2ang_ring (https://healpix.sourceforge.io/)
long nside2order(unsigned int nside);
unsigned int nside2npix(unsigned int nside);
QDirection pix2ang_ring(unsigned int nside, unsigned int ipix);
unsigned int ang2pix_ring(unsigned int nside, const QDirection &thetaphi);
unsigned int loc2pix(unsigned int nside, double z, double phi, double sth,
                     bool have_sth);

}  // namespace hermes

#endif  // HERMES_HEALPIXBITS_H

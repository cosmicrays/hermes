#ifndef HERMES_UNITSHERMES_H
#define HERMES_UNITSHERMES_H

#include "UnitsDerived.h"

namespace hermes {

// HERMES specific types
// ------------------------------------
//             l  t  m  I  T  N  J  A
QUANTITY_TYPE( 2, 0, 0, 1, 0, 0, 0, 0, QSynchroConstant);
QUANTITY_TYPE(-2, 1,-1, 0, 0, 0, 0, 0, QPiZeroIntegral);
QUANTITY_TYPE( 1,-1, 0, 0, 1, 0, 0, 0, QRingCOIntensity);
QUANTITY_TYPE(-3, 1, 0, 0,-1, 0, 0, 0, QRingX0Unit);
QUANTITY_TYPE(-3, 2,-1, 0, 0, 0, 0, 0, QICInnerIntegral);
QUANTITY_TYPE(-5, 1,-1, 0, 0, 0, 0, 0, QICOuterIntegral);

} // namespace hermes

#endif // HERMES_UNITSHERMES_H

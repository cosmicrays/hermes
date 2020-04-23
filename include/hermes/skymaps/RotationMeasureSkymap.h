#ifndef HERMES_ROTATIONMEASURESKYMAP_H
#define HERMES_ROTATIONMEASURESKYMAP_H

#include <hermes/skymaps/SkymapTemplate.h>

namespace hermes {
/**
 * \addtogroup Skymaps
 * @{
 */

/**
 @class RotationMeasureSkymap
 @brief A skymap container for rotation measure (RM).
 */

typedef SkymapTemplate<QRotationMeasure, QNumber> RotationMeasureSkymap;

/** @}*/

}  // namespace hermes
#endif  // HERMES_ROTATIONMEASURESKYMAP_H

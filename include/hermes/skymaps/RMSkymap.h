#ifndef HERMES_RMSKYMAP_H
#define HERMES_RMSKYMAP_H

#include <hermes/skymaps/SkymapTemplate.h>

namespace hermes {
/**
 * \addtogroup Skymaps
 * @{
 */

/**
 @class RMSkymap
 @brief A skymap container for rotation measure (RM).
 */

typedef SkymapTemplate<QRotationMeasure, QNumber> RMSkymap;

/** @}*/
} // namespace hermes
#endif // HERMES_RMSKYMAP_H

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
class RotationMeasureSkymap : public SkymapTemplate<QRotationMeasure, QNumber> {
  private:
  public:
	RotationMeasureSkymap(std::size_t nside) : SkymapTemplate(nside) {
		initDefaultOutputUnits(1_rad / 1_m2, "rad m^-2");
	};
};

/** @}*/

}  // namespace hermes
#endif  // HERMES_ROTATIONMEASURESKYMAP_H

#ifndef HERMES_DISPERSIONMEASURESKYMAP_H
#define HERMES_DISPERSIONMEASURESKYMAP_H

#include <hermes/skymaps/SkymapTemplate.h>

namespace hermes {

/**
 * \addtogroup Skymaps
 * @{
 */

/**
 @class DispersionMeasureSkymap
 @brief A skymap container for dispersion measure (DM).
 */
class DispersionMeasureSkymap
    : public SkymapTemplate<QDispersionMeasure, QNumber> {
  private:
  public:
    DispersionMeasureSkymap(std::size_t nside_) : SkymapTemplate(nside_) {

	initDefaultOutputUnits(parsec / centimetre3, "parsec^-1 cm^3");
    };
};
/** @}*/

} // namespace hermes
#endif // HERMES_DISPERSIONMEASURESKYMAP_H

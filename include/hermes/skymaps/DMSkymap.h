#ifndef HERMES_DMSKYMAP_H
#define HERMES_DMSKYMAP_H

#include <hermes/skymaps/SkymapTemplate.h>

namespace hermes {

/**
 * \addtogroup Skymaps
 * @{
 */

/**
 @class DMSkymap
 @brief A skymap container for dispersion measure (DM).
 */
class DMSkymap: public SkymapTemplate<QDispersionMeasure, QNumber> {
private:
public:
        DMSkymap(std::size_t nside_) :
                SkymapTemplate(nside_) {

                initDefaultOutputUnits(parsec/centimetre3, "parsec^-1 cm^3");
        };
};
/** @}*/
} // namespace hermes
#endif // HERMES_DMSKYMAP_H

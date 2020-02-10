#ifndef HERMES_DMSKYMAP_H
#define HERMES_DMSKYMAP_H

#include <hermes/skymaps/SkymapTemplate.h>

namespace hermes {

class DMSkymap: public SkymapTemplate<QDispersionMeasure, QNumber> {
private:
public:
        DMSkymap(std::size_t nside_) :
                SkymapTemplate(nside_) {

                initDefaultUnits(parsec/centimetre3, "parsec^-1 cm^3");
        };
};


} // namespace hermes

#endif // HERMES_DMSKYMAP_H

#ifndef HERMES_HIICORDES91_H
#define HERMES_HIICORDES91_H

#include "hermes/gasDensity/GasDensity.h"

namespace hermes {

class HII_Cordes91: public GasDensity {
private:
	QPDensity fne1, fne2;
	QLength	  H1, A1, H2, A2, R2;
public:
	HII_Cordes91();
	QPDensity getDensity(const Vector3Length& pos) const;
};

} // namespace hermes

#endif // HERMES_HIICORDES91_H

#ifndef HERMES_HIICORDES91_H
#define HERMES_HIICORDES91_H

#include "hermes/chargedGasDensity/ChargedGasDensity.h"

namespace hermes {

class HII_Cordes91: public ChargedGasDensity {
private:
	QPDensity fne1, fne2;
	QLength	  H1, A1, H2, A2, R2;
public:
	HII_Cordes91();
	QPDensity getDensity(const Vector3QLength& pos) const;
};

} // namespace hermes

#endif // HERMES_HIICORDES91_H

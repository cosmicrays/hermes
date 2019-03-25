#ifndef HERMES_YMW16_H
#define HERMES_YMW16_H

#include "hermes/gasDensity/GasDensity.h"

extern "C" {
	#include "cn.h"
}

namespace hermes {

class YMW16: public GasDensity {
private:
	QPDensity fne1, fne2;
	QLength	  H1, A1, H2, A2, R2;
public:
	YMW16();
	QPDensity getDensity(const Vector3QLength& pos) const;
};

} // namespace hermes

#endif // HERMES_YMW16_H

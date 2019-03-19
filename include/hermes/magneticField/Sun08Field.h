#ifndef HERMES_SUN08FIELD_H
#define HERMES_SUN08FIELD_H

#include "hermes/magneticField/MagneticField.h"

namespace hermes {

/**
 @class Sun08Field
 @brief 3-year WMAP galactic magnetic field model

 Implemented the galactic magnetic field model used in 3-year WMAP analyses. 

 See: L. Page et al 2007 ApJS 170 335, https://iopscience.iop.org/article/10.1086/513699
 */

class Sun08Field: public MagneticField {
private:
	QLength R_Earth, R_0, z_0, R_c;
	QMField B_0, B_c;
	QLength z_H0, z_H1, R_H0;
	QMField B_H0;
	QAngle pitch;

public:
	Sun08Field();
	Vector3QMField getField(const Vector3QLength& pos) const;
};

} // namespace hermes

#endif // HERMES_SUN08FIELD_H

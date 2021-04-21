#ifndef HERMES_WMAP07_H
#define HERMES_WMAP07_H

#include "hermes/magneticfields/MagneticField.h"

namespace hermes { namespace magneticfields {
/**
 * \addtogroup MagneticFields
 * @{
 */

/**
 @class WMAP07
 @brief 3-year WMAP galactic magnetic field model

 The implemention of the galactic magnetic field model used in 3-year WMAP analyses.
 Note the distance from the galactic centre 8 kpc.

 See: L. Page et al 2007 ApJS 170 335,
 https://iopscience.iop.org/article/10.1086/513699
 */

class WMAP07 : public MagneticField {
  private:
	QMField B_0;
	QLength r_0;
	QLength z_0;
	QAngle chi_0;
	QAngle psi_1;
	QAngle psi_0;

  public:
	WMAP07();
	Vector3QMField getField(const Vector3QLength &pos) const override;
};

/** @} */
}}  // namespace hermes::magneticfields

#endif  // HERMES_WMAP07_H

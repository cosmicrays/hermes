#ifndef HERMES_COMMON_H
#define HERMES_COMMON_H

#include "hermes/Units.h"

/**
 @file
 @brief Common helper functions
 */

namespace hermes {

	/**
 		Convert from spherical to right-handed cartesian coordinates
		Physical convention (r,theta,phi); latitude - theta, longitude - phi
	*/	
	inline Vector3QLength sphericalToCartesian(QLength r, QAngle theta, QAngle phi) {
		Vector3QLength v;
		v.x = r * sin(theta) * cos(phi);
		v.y = r * sin(theta) * sin(phi);
		v.z = r * cos(theta);
		return v;
	}

} // namespace hermes

#endif // HERMES_COMMON_H

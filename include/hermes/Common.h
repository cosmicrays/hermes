#ifndef HERMES_COMMON_H
#define HERMES_COMMON_H

#include "hermes/Units.h"

#include <iostream>
#include <array>

/**
 @file
 @brief Common helper functions
 */

namespace hermes {

	typedef std::array<QAngle, 2> QDirection;

	/**
		Gives a distance from the galactic centre (GC) by providing the distance 
		from the Sun and the direction (theta,phi)
	*/
	inline QLength distanceFromGC(QDirection direction, QLength distanceFromSun,
					Vector3QLength vecGCSun) {
		Vector3QLength vecSunTarget;
		vecSunTarget.setRThetaPhi(distanceFromSun, direction[0], direction[1]);
		Vector3QLength vecGCTarget = vecSunTarget - vecGCSun;

		return vecGCTarget.getR();
	}

        /**
 		Gives a distance from an observer to the (spherical) galactic border
		in the given direction (law of cosines)
        */
	inline QLength distanceToGalBorder(Vector3QLength positionSun, QDirection direction) {
	
		Vector3QLength positionGC(0, 0, 0);
		QLength galacticBorder = 20_kpc; // JF12 is zero for r > 20kpc
		Vector3QLength vecSunToGalBorder;
	        vecSunToGalBorder.setRThetaPhi(1_m, direction[0], direction[1]);

        	QLength a = (positionSun-positionGC).getR();
	        QLength c = galacticBorder;
        	QAngle gamma = vecSunToGalBorder.getAngleTo(positionSun-positionGC);
	        return (2*a*cos(gamma) + std::sqrt(2)*sqrt(2*c*c - a*a + a*a*cos(2*gamma)))/2.0;
	}

	/**
	 	Calculate the Lorentz factor (gamma) from a particle mass and energy
	*/
	inline QNumber getLorentzFactor(QMass m, QEnergy E) {
	        return E / (m * c_squared);
	}

} // namespace hermes

#endif // HERMES_COMMON_H

#ifndef HERMES_COMMON_H
#define HERMES_COMMON_H

#include "hermes/Units.h"
#include "hermes/Vector3Quantity.h"

/**
 @file
 @brief Common helper functions
 */

namespace hermes {

/**
	Gives a distance from the galactic centre (GC) by providing the distance 
	from the Sun and the direction (theta,phi)
*/
QLength distanceFromGC(QDirection direction, QLength distanceFromSun,
				Vector3QLength vecGCSun);

/**
	Gives a distance from an observer to the (spherical) galactic border
	in the given direction (law of cosines)
*/
QLength distanceToGalBorder(Vector3QLength positionSun, QDirection direction);

/**
	Test if two directions are close one another, within d

*/
bool isWithinAngle(QDirection a, QDirection b, QAngle d);

/**
 	Calculate the Lorentz factor (gamma) from a particle mass and energy
*/
QNumber getLorentzFactor(QMass m, QEnergy E);

} // namespace hermes

#endif // HERMES_COMMON_H

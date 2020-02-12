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
	Resolves a full path to data files
*/
std::string getDataPath(std::string filename);

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
	Returns XYZ position in the (right-handed) galacto-centric
	coordinate system by giving distance and direction from the observer
*/
inline Vector3QLength getGalacticPosition(const Vector3QLength &posSun,
		const QLength &dist, const QDirection &dir) {
	Vector3QLength pos(0);

	// TODO: should be more general for any observer position
	pos.setRThetaPhi(dist, dir[0], dir[1]);
	pos.x = posSun.x - pos.x;
	pos.y = -pos.y;

	return pos;
}
/**
	Test if two directions are close one another, within d

*/
bool isWithinAngle(QDirection a, QDirection b, QAngle d);

/**
 	Calculate the Lorentz factor (gamma) from a particle mass and energy
*/
QNumber getLorentzFactor(QMass m, QEnergy E);

/**
 	Intensity to temperature coversion
*/
QTemperature intensityToTemperature(QIntensity intensity, QFrequency freq);


} // namespace hermes

#endif // HERMES_COMMON_H

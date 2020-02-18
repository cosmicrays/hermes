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
QLength distanceToGalBorder(const Vector3QLength &positionSun, const QDirection &direction,
		const QLength &galacticBorder = 30_kpc, const QLength &zBorder = 5_kpc);

/**
	Returns XYZ position in the (right-handed) galacto-centric
	coordinate system by giving distance and direction from the observer
*/
Vector3QLength getGalacticPosition(const Vector3QLength &posSun,
		const QLength &dist, const QDirection &dir);

/**
	Conversion to/from Galactic coordinates from/to angles of spherical coordinate system (ISO)
*/
QDirection toGalCoord(const QDirection &);
QDirection fromGalCoord(const QDirection &);

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

/**
	Returns number of available system threads
*/
int getThreadsNumber();

/**
	Get thread ID
*/
size_t getThreadId();

/**
	For a given number of tasks, split them over available system threads;
	returns a vector of [start, stop> pairs
*/
std::vector<std::pair<int,int>> getThreadChunks(int queueSize);

} // namespace hermes

#endif // HERMES_COMMON_H

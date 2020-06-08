#ifndef HERMES_COMMON_H
#define HERMES_COMMON_H

#include <gsl/gsl_integration.h>

#include <functional>

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
std::string getDataPath(const std::string &filename);

/**
    Gives a distance from the galactic centre (GC) by providing the distance
    from the Sun and the direction (theta,phi)
*/
QLength distanceFromGC(QDirection direction, QLength distanceFromSun,
                       const Vector3QLength &vecGCSun);

/**
    Gives a distance from an observer to the (spherical) galactic border
    in the given direction (law of cosines)
*/
QLength distanceToGalBorder(const Vector3QLength &positionSun,
                            const QDirection &direction,
                            const QLength &galacticBorder = 35_kpc,
                            const QLength &zBorder = 5_kpc);

/**
    Returns XYZ position in the (right-handed) galacto-centric
    coordinate system by giving distance and direction from the observer
*/
Vector3QLength getGalacticPosition(const Vector3QLength &posSun,
                                   const QLength &dist, const QDirection &dir);

/**
    Conversion to/from Galactic coordinates from/to angles of spherical
   coordinate system (ISO)
*/
QDirection toGalCoord(const QDirection &);
QDirection fromGalCoord(const QDirection &);

/**
    Test if two directions are close one another, within d

*/
bool isWithinAngle(const QDirection &a, const QDirection &b, const QAngle &d);

/**
    Calculate the Lorentz factor (gamma) from a particle mass and energy
*/
QNumber getLorentzFactor(const QMass &m, const QEnergy &E);

/**
    Intensity to temperature coversion
*/
QTemperature intensityToTemperature(const QIntensity &intensity,
                                    const QFrequency &freq);

/**
    Returns number of available system threads
*/
unsigned int getThreadsNumber();

/**
    Get thread ID
*/
size_t getThreadId();

/**
    For a given number of tasks, split them over available system threads;
    returns a vector of [start, stop> pairs
*/
std::vector<std::pair<unsigned int, unsigned int>> getThreadChunks(
    unsigned int queueSize);

/* Template wrapper to expose lambda with capture to gsl_function
 * according to https://stackoverflow.com/a/18413206/6819103 */
template <typename F>
class gsl_function_pp : public gsl_function {
  private:
	const F &_func;
	static double invoke(double x, void *params) {
		return static_cast<gsl_function_pp *>(params)->_func(x);
	}

  public:
	gsl_function_pp(const F &func) : _func(func) {
		function = &gsl_function_pp::invoke;
		params = this;
	}
};

}  // namespace hermes

#endif  // HERMES_COMMON_H

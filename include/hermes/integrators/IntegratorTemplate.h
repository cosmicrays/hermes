#ifndef HERMES_INTEGRATOR_H
#define HERMES_INTEGRATOR_H

#include <gsl/gsl_integration.h>

#include <memory>
#include <vector>

#include "hermes/Common.h"
#include "hermes/Grid.h"
#include "hermes/HEALPixBits.h"
#include "hermes/Units.h"

/**
 \file IntegratorTemplate.h
 \brief Contains \p IntegratorTemplate definition used in all
 integrators.
 */

namespace hermes {
/**
 \addtogroup Integrators
 @{
 */

/**
 \class IntegratorTemplate
 \brief Defines the main integrator interface and typical methods shared
 across all integrators.
 \tparam QPXL A type of pixel which an integrator returns (e.g.,
 units::QTemperature, units::QDiffIntensity)
 \tparam QSTEP A physical quantity (a parameter) that describes a specific map
 (e.g., units::QFrequency, units::QEnergy)
 */
template <class QPXL, typename QSTEP>
class IntegratorTemplate {
  protected:
	Vector3QLength observerPosition;
	QSTEP skymapParameter;
	bool cacheEnabled;
	bool cacheTableInitialized;
	std::string description;

  public:
	IntegratorTemplate(const std::string &description)
	    : observerPosition(Vector3QLength(8.5_kpc, 0, 0)),
	      cacheEnabled(false),
	      cacheTableInitialized(false),
	      description(description){};
	virtual ~IntegratorTemplate() {}

	/** Get Description of the integrator */
	std::string getDescription() const { return description; }
	/** Set Description of the integrator */
	void setDescription(const std::string &description) {
		this->description = description;
	}

	/**
	    Setter for the skymap parameter
	    (requires for the cacheTable, if enabled, to be re-initialized)
	*/
	void setSkymapParameter(const QSTEP &p) {
		if (skymapParameter != p) {
			skymapParameter = p;
			cacheTableInitialized = false;
		}
	}
	/**
	    Getter for the skymap parameter
	*/
	QSTEP setSkymapParameter() const { return skymapParameter; }
	/**
	 * Every instantiation/derived class must implement this method which
	 * represents an integral over the line of sight of an accumulated
	 * quantity \f$ f(s)\f$ for a given units::QDirection \p dir. When
	 * multiplied with ::units::QLength gives the pixel type, \p QPXL
	 * @f[
	 * I(\mathrm{dir}) \sim \int_0^\infty \mathrm{d}s f(s)
	 * @f]
	 */
	virtual QPXL integrateOverLOS(const QDirection &dir) const = 0;

	/**
	 *  Additionally, for a frequency or energy dependent integrals one
	 *  should implement the following method too.
	 */
	virtual QPXL integrateOverLOS(const QDirection &dir,
	                              const QSTEP &) const = 0;

	/**
	    Set the position of the Sun in the galaxy as a vector (x, y, z)
	   from which the LOS integration starts, default: (8.5_kpc, 0, 0)
	*/
	void setObsPosition(const Vector3QLength &pos) { observerPosition = pos; }
	/**
	    Get the position of the Sun in the galaxy as a vector (x, y, z)
	*/
	inline Vector3QLength getObsPosition() const { return observerPosition; }
	/**
	    Wrapper within the class for distanceToGalBorder(observerPosition,
	   direction) becomes getMaxDistance(direction)
	*/
	inline QLength getMaxDistance(const QDirection &direction) const {
		return distanceToGalBorder(observerPosition, direction);
	}
	/**
	    Caching helpers
	*/
	virtual void setupCacheTable(int N_x, int N_y, int N_z){};
	virtual void initCacheTable(){};
	bool isCacheTableEnabled() const { return cacheEnabled; };
	bool isCacheTableInitialized() const { return cacheTableInitialized; };

	/**
	   Get the line of sight profile (integrand of integrateOverLOS) of
	   a direction where N is equidistant number of steps from the Sun's
	   position to the galactic border
	*/
	typedef std::pair<std::vector<QLength>, std::vector<double>> tLOSProfile;
	virtual tLOSProfile getLOSProfile(const QDirection &dir, int Nsteps) const {
		return tLOSProfile();
	}
	virtual tLOSProfile getLOSProfile(const QDirection &dir, const QSTEP &,
	                                  int Nsteps) const {
		return tLOSProfile();
	}
};

typedef IntegratorTemplate<QDispersionMeasure, QNumber>
    DispersionMeasureIntegratorTemplate;
typedef IntegratorTemplate<QRotationMeasure, QNumber>
    RotationMeasureIntegratorTemplate;
typedef IntegratorTemplate<QTemperature, QFrequency> RadioIntegratorTemplate;
typedef IntegratorTemplate<QDiffIntensity, QEnergy> GammaIntegratorTemplate;

/** @}*/
}  // namespace hermes

#endif  // HERMES_INTEGRATOR_H

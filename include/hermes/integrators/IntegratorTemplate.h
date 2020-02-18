#ifndef HERMES_INTEGRATOR_H
#define HERMES_INTEGRATOR_H

#include "hermes/Units.h"
#include "hermes/Grid.h"
#include "hermes/HEALPixBits.h"
#include "hermes/Common.h"

#include <vector>
#include <memory>
#include <gsl/gsl_integration.h>

namespace hermes {
/**
 * \addtogroup Integrators
 * @{
 */

/**
 @class IntegratorTemplate
 @brief Provides the integrator interface and implements methods shared across integrators.
 * \tparam QPXL A type of pixel which an integrator returns (for example, QTemperature, QIntensity)
 * \tparam QSTEP A physical quantity (a parameter) that describes a specific map (e.g., QFrequency, QEnergy)
 */

template <class QPXL, typename QSTEP>
class IntegratorTemplate {
protected:
	Vector3QLength positionSun;
	QSTEP skymapParameter;
	bool cacheEnabled;
	bool cacheTableInitialized;
public:
	IntegratorTemplate() :
		positionSun(Vector3QLength(8.5_kpc, 0, 0)),
       		cacheEnabled(false), cacheTableInitialized(false) { };
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
	QSTEP setSkymapParameter() const {
		return skymapParameter;	
	}
	/**
 		Every child class should implement this method which represents an integral
		of a targeted accumulated quantity `T` in a given direction `interdir`.
	*/
	virtual QPXL integrateOverLOS(QDirection iterdir) const { return  QPXL(0); };
	/**
		Additionally, for a frequency or energy dependent integrals one should
		implement the following method too.
	*/
	virtual QPXL integrateOverLOS(QDirection iterdir, QSTEP) const { return QPXL(0); };
	/**
	 	Set the position of the Sun in the galaxy as a vector (x, y, z) from which
		the LOS integration starts, default: (8.5_kpc, 0, 0)
	*/
	void setSunPosition(Vector3QLength pos_) {
		positionSun = pos_;
	}
	/**
	 	Get the position of the Sun in the galaxy as a vector (x, y, z)
	*/
	inline Vector3QLength getSunPosition() const {
		return positionSun;
	}
	/**
		Wrapper within the class for distanceToGalBorder(positionSun, direction)
		becomes getMaxDistance(direction)
	*/
	inline QLength getMaxDistance(QDirection direction) const {
		return distanceToGalBorder(positionSun, direction);
	}
	/**
		Caching helpers
	*/
       	virtual void setupCacheTable(int N_x, int N_y, int N_z)	{ };
	virtual void initCacheTable() { };
	bool isCacheTableEnabled() const {
		return cacheEnabled;
	};
	bool isCacheTableInitialized() const {
		return cacheTableInitialized;
	};
};

typedef IntegratorTemplate<QTemperature, QFrequency> RadioIntegratorTemplate;
typedef IntegratorTemplate<QDifferentialIntensity, QEnergy> GammaIntegratorTemplate;

/** @}*/
} // namespace hermes

#endif // HERMES_INTEGRATOR_H
#ifndef HERMES_GALACTICMAGNETICFIELD_H
#define HERMES_GALACTICMAGNETICFIELD_H

#include <cmath>

#include "hermes/magneticfields/MagneticField.h"

namespace hermes { namespace magneticfields {
/**
 * \addtogroup MagneticFields
 * @{
 */

/**
 @class TorroidalHaloField
 @brief Galactic halo field model from Prouza & Smida 2003 and Sun et al. 2008
 */
class TorroidalHaloField : public MagneticField {
	double b0;  // halo field strength
	double z0;  // vertical position
	double z1;  // vertical scale
	double r0;  // radial scale

  public:
	void setParameters(double b0, double z0, double z1, double r0) {
		this->b0 = b0;
		this->z0 = z0;
		this->z1 = z1;
		this->r0 = r0;
	}

	Vector3d getField(Vector3d pos) {
		double r = sqrt(pos.x * pos.x + pos.y * pos.y) /
		           r0;  // in-plane radius in units of the radial scale
		double b =
		    b0 / (1 + pow((std::fabs(pos.z) - z0) / z1, 2)) * r * exp(1 - r);
		double phi = pos.getPhi();  // azimuth
		return Vector3d(cos(phi), sin(phi), 0) * b;
	}
};

/**
 @class LogarithmicSpiralField
 @brief Galactic disk field model of axisymmetric (ASS) or bisymmetric (BSS)
 logarithmic spiral shape
 */
class LogarithmicSpiralField : public MagneticField {
  private:
	bool isBSS;    // true for BSS, false for ASS
	double b0;     // field strength
	double pitch;  // pitch angle [rad]
	double rsol;   // distance of sun to galactic center
	double rc;     // radius of central region with constant field strength
	double d;      // distance to the first field reversal
	double z0;     // vertical attenuation length

	double phase;  // phase of the spiral arms
	double cosPhase;
	double sinPitch;
	double cosPitch;
	double tanPitch;

	void updatePitchAngle() {
		sinPitch = sin(pitch);
		cosPitch = cos(pitch);
		tanPitch = tan(pitch);
	}

	void updatePhase() {
		phase = log(1 + d / rsol) / tanPitch - M_PI / 2;
		cosPhase = cos(phase);
	}

  public:
	void setParameters(bool isBSS, double b0, double pitch, double rsol,
	                   double rc, double d, double z0) {
		this->isBSS = isBSS;
		this->b0 = b0;
		this->pitch = pitch;
		this->rsol = rsol;
		this->rc = rc;
		this->d = d;
		this->z0 = z0;
		updatePitchAngle();
		updatePhase();
	}

	Vector3d getField(Vector3d pos) const {
		double r = sqrt(pos.x * pos.x + pos.y * pos.y);  // in-plane radius
		double b = b0 / cosPhase * rsol / std::max(r, rc);

		double phi = pos.getPhi();
		double c = cos(phi - log(r / rsol) / tanPitch + phase);
		if (isBSS) c = std::fabs(c);
		b *= c * exp(std::fabs(pos.z) / z0);

		return Vector3d(cosPitch * cos(phi), sinPitch * sin(phi), 0) * b;
	}
};

/** @} */
}}  // namespace hermes::magneticfields

#endif  // HERMES_GALACTICMAGNETICFIELD_H

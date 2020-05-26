#ifndef HERMES_JF12FIELD_H
#define HERMES_JF12FIELD_H

#include <memory>

#include "hermes/Grid.h"
#include "hermes/GridTools.h"
#include "hermes/Units.h"
#include "hermes/magneticfields/MagneticField.h"

namespace hermes { namespace magneticfields {
/**
 * \addtogroup MagneticFields
 * @{
 */

/**
 @class JF12Field
 @brief JF12Field galactic magnetic field model

 Implements the JF2012 magnetic field model, consisting of a large-scale regular
 and random (striated) field and a small-scale random (turbulent) field.
 See:
 Jansson 2012a, ApJ. 757, A New Model of the Galactic Magnetic Field
 Jansson 2012b, arXiv:1210.7820, The Galactic Magnetic Field

 All three components may individually turned on and off.
 Currently only best fit values of the field paramaters are implemented and
 cannot be changed.

 The field is defined in the usual galactocentric coordinate system with the
 Galactic center at the origin, the x-axis pointing in the direction of
 the Sun, and the z-axis pointing towards Galactic north.
 */
class JF12Field : public MagneticField {
  private:
	bool useRegular;
	bool useStriated;
	bool useTurbulent;

	// disk spiral arms
	QLength rArms[8];  // radii where each arm crosses the negative x-axis
	QAngle pitch;      // pitch angle
	double sinPitch, cosPitch, tan90MinusPitch;

	// Regular field
	// ---------------------------------------------------------- disk
	QMField bDisk[8];      // field strengths of arms at r=5 kpc
	QMField bRing;         // ring field strength 3<r<5 kpc
	QLength hDisk, wDisk;  // disk/halo transistion and width
	// toroidal halo
	QMField bNorth, bSouth;  // northern, southern halo field strength
	QLength rNorth, rSouth;  // northern, southern transistion radius
	QLength wHalo, z0;       // transistion width and vertical scale height
	// poloidal halo
	QMField bX;      // field strength at origin
	QAngle thetaX0;  // constant elevation angle at r > rXc, z = 0
	double sinThetaX0, cosThetaX0, tanThetaX0;
	QLength rXc;  // radius of varying elevation angle region
	QLength rX;   // exponential scale height

	// Striated field
	// ---------------------------------------------------------
	double sqrtbeta;  // relative strength of striated field
	std::shared_ptr<ScalarGrid> striatedGrid;

	// Turbulent field
	// --------------------------------------------------------
	std::shared_ptr<VectorGrid> turbulentGrid;
	// disk
	QMField bDiskTurb[8];  // field strengths in arms at r=5 kpc
	QMField bDiskTurb5;    // field strength at r<5kpc
	QLength zDiskTurb;     // Gaussian scale height of disk
	// halo
	QMField bHaloTurb;  // halo field strength
	QLength rHaloTurb;  // exponential scale length
	QLength zHaloTurb;  // Gaussian scale height

  public:
	JF12Field();

	// Create and set a random realization for the striated field
	void randomStriated(int seed = 0);

#ifdef HERMES_HAVE_FFTW3F
	// Create a random realization for the turbulent field
	void randomTurbulent(int seed = 0);
#endif

	/**
	 * Set a striated grid and activate the striated field component
	 * @param grid	scalar grid containing random +1/-1 values, 100 parsec
	 * grid spacing
	 */
	void setStriatedGrid(std::shared_ptr<ScalarGrid> grid);

	/**
	 * Set a turbulent grid and activate the turbulent field component
	 * @param grid	vector grid containing a random field of Brms = 1
	 */
	void setTurbulentGrid(std::shared_ptr<VectorGrid> grid);

	std::shared_ptr<ScalarGrid> getStriatedGrid();
	std::shared_ptr<VectorGrid> getTurbulentGrid();

	void setUseRegular(bool use);
	void setUseStriated(bool use);
	void setUseTurbulent(bool use);

	bool isUsingRegular() const;
	bool isUsingStriated() const;
	bool isUsingTurbulent() const;

	// Regular field component
	Vector3QMField getRegularField(const Vector3QLength &pos) const;

	// Regular and striated field component
	Vector3QMField getStriatedField(const Vector3QLength &pos) const;

	// Brms of the turbulent field
	QMField getTurbulentStrength(const Vector3QLength &pos) const;

	// Turbulent field component
	Vector3QMField getTurbulentField(const Vector3QLength &pos) const;

	// All set field components
	Vector3QMField getField(const Vector3QLength &pos) const override;
};

/** @} */
}}  // namespace hermes::magneticfields

#endif  // HERMES_JF12FIELD_H

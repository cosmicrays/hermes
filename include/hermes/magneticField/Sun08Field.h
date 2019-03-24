#ifndef HERMES_SUN08FIELD_H
#define HERMES_SUN08FIELD_H

#include "hermes/magneticField/MagneticField.h"
#include "hermes/Grid.h"
#include "hermes/GridTools.h"

namespace hermes {

/**
 @class Sun08Field
 @brief 3-year WMAP galactic magnetic field model

 Implemented the galactic magnetic field model used in 3-year WMAP analyses. 

 See: L. Page et al 2007 ApJS 170 335, https://iopscience.iop.org/article/10.1086/513699
 */

class Sun08Field: public MagneticField {
private:
	bool useTurbulent;
	std::shared_ptr<VectorGrid> turbulentGrid;

	QLength R_Earth, R_0, z_0, R_c;
	QMField B_0, B_c, B_turbulent;
	QLength z_H0, z_H1, R_H0;
	QMField B_H0;
	QAngle pitch;

public:
	Sun08Field();

#ifdef HERMES_HAVE_FFTW3F
	// Create a random realization for the turbulent field
	void randomTurbulent(int seed = 0);
#endif
	
	/**
	 * Set a turbulent grid and activate the turbulent field component
	 * @param grid	vector grid containing a random field of Brms = 1
	 */
	void setTurbulentGrid(std::shared_ptr<VectorGrid> grid);

	std::shared_ptr<VectorGrid> getTurbulentGrid();

	void setUseTurbulent(bool use);
	bool isUsingTurbulent() const;

	// Regular field component
	Vector3QMField getRegularField(const Vector3QLength& pos) const;

	// Turbulent field component
	Vector3QMField getTurbulentField(const Vector3QLength& pos) const;

	// Get the total field
	Vector3QMField getField(const Vector3QLength& pos) const;

};

} // namespace hermes

#endif // HERMES_SUN08FIELD_H

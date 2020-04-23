#ifndef HERMES_SUN08FIELD_H
#define HERMES_SUN08FIELD_H

#include "hermes/Grid.h"
#include "hermes/GridTools.h"
#include "hermes/magneticfields/MagneticField.h"

namespace hermes { namespace magneticfields {

/**
 @class Sun08Field
 @brief

 Implemented the galactic magnetic field model used in 3-year WMAP analyses.

 See: L. Page et al 2007 ApJS 170 335,
 https://iopscience.iop.org/article/10.1086/513699
 */

class Sun08Field : public MagneticField {
  private:
	bool useTurbulent;
	bool useHalo;
	std::shared_ptr<VectorGrid> turbulentGrid;

	QLength R_Sun;
	// disk parameters
	QLength disk_z_0, disk_R_c;
	QLength disk_R_0, disk_R_1, disk_R_2, disk_R_3;
	QMField disk_B_0, disk_B_c, disk_B_turbulent;
	// halo parameters
	QLength halo_z_0, halo_z_1, halo_R_0;
	QMField halo_B_0;
	QAngle pitch;
	QMField B_turbulent;

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
	void setUseHalo(bool use);
	bool isUsingTurbulent() const;
	bool isUsingHalo() const;

	// Regular field component
	Vector3QMField getRegularField(const Vector3QLength &pos) const;

	// Turbulent field component
	Vector3QMField getTurbulentField(const Vector3QLength &pos) const;

	// Get the total field
	Vector3QMField getField(const Vector3QLength &pos) const override;
};

}}  // namespace hermes::magneticfields

#endif  // HERMES_SUN08FIELD_H

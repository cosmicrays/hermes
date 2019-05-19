#ifdef HERMES_HAVE_CFITSIO

#include "hermes/neutralGasDensity/RingModelDensity.h"
#include "hermes/Common.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace hermes {

RingModelDensity::RingModelDensity() {
	readDataFiles();
}

void RingModelDensity::readDataFiles() {
	ffile = std::make_unique<FITSFile>(
			FITSFile(getDataPath("RingModelDensity/NHrings_Ts300K.fits"))); 
	ffile->openFile(FITS::READ);

	n_lon = ffile->readKeyValueAsInt("NAXIS1");
	n_lat = ffile->readKeyValueAsInt("NAXIS2");
	n_rings = ffile->readKeyValueAsInt("NAXIS3");
	
	min_lon = ffile->readKeyValueAsDouble("CRVAL1");
	delta_lon = ffile->readKeyValueAsDouble("CDELT1");
	min_lat = ffile->readKeyValueAsDouble("CRVAL2");
	delta_lat = ffile->readKeyValueAsDouble("CDELT2");
	
	std::cerr << "Number of rings:" << n_rings << std::endl;

	int firstElement = 1;
	int nElements = n_lon*n_lat*n_rings;
	dataVector = ffile->readImageAsFloat(firstElement, nElements);
}

QPDensity RingModelDensity::getDensityInRing(int ring, const QDirection& dir) const {
	QAngle lat = dir[0];
    	QAngle lon = dir[1] + 180_deg; // becaue the galactic centre of
				       // the ring model is the middle of the image

	int pxl_lat = static_cast<int>(
			std::round(
				static_cast<double>((lat / 180_deg) * n_lat))
			);
	int pxl_lon = static_cast<int>(
			std::round(
				static_cast<double>((lon / 360_deg) * n_lon))
			);

	// NAXIS1 x NAXIS2 x NAXIS3 => lon x lat x ring
	return dataVector[
		(ring * n_lat + pxl_lat) * n_lon + pxl_lon
	];
}

} // namespace hermes

#endif // HERMES_HAVE_CFITSIO

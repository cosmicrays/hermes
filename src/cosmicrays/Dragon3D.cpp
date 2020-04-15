#ifdef HERMES_HAVE_CFITSIO

#include "hermes/Common.h"
#include "hermes/cosmicrays/Dragon3D.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#define DEFAULT_CR_FILE "CosmicRays/Fornieri20/run2d_gamma_D03,7_delta0,45_vA13.fits.gz"

namespace hermes { namespace cosmicrays {

Dragon3D::Dragon3D(
		const std::string& filename_, const PID& pid_)
    : CosmicRayDensity(pid_), filename(filename_) {
	readFile();
}

Dragon3D::Dragon3D(
		const std::string& filename_, const std::vector<PID> &pids_)
    : CosmicRayDensity(pids_), filename(filename_) {
	readFile();
}

void Dragon3D::readFile() {
	ffile = std::make_unique<FITSFile>(FITSFile(filename)); 
  
	ffile->openFile(FITS::READ);
	ffile->moveToHDU(1);
	
	// read header
	readEnergyAxis();
	
	readSpatialGrid3D();
	readDensity3D();
}

QPDensityPerEnergy Dragon3D::getDensityPerEnergy(
		const QEnergy &E_, const Vector3QLength& pos_) const {
    	return getDensityPerEnergy(static_cast<int>(energyIndex.at(E_)), pos_);
}

QPDensityPerEnergy Dragon3D::getDensityPerEnergy(
		int iE_, const Vector3QLength& pos_) const {
	if (pos_.z < zmin || pos_.z > zmax)
		return QPDensityPerEnergy(0);
	
	QLength rho = sqrt(pos_.x*pos_.x + pos_.y*pos_.y);
	if (rho > rmax)
		return QPDensityPerEnergy(0);

    	return (grid[iE_])->interpolate(pos_);
}

void Dragon3D::readEnergyAxis() {
    	QEnergy E;
	double Ekmin = ffile->readKeyValueAsDouble("Ekmin");
	dimE = ffile->readKeyValueAsInt("dimE");
	energyScaleFactor = ffile->readKeyValueAsDouble("Ekin_fac");

	// input files are in GeV	
	for (int i = 0; i < dimE; ++i) {
		E = 1_GeV * std::exp(std::log(Ekmin) +
			static_cast<double>(i) * std::log(energyScaleFactor));
    		energyRange.push_back(E);
		energyIndex[E] = i;
	}
}

void Dragon3D::readSpatialGrid3D() {

	xmin = ffile->readKeyValueAsDouble("xmin") * 1_kpc;
	xmax = ffile->readKeyValueAsDouble("xmax") * 1_kpc;
	ymin = ffile->readKeyValueAsDouble("ymin") * 1_kpc;
	ymax = ffile->readKeyValueAsDouble("ymax") * 1_kpc;
	zmin = ffile->readKeyValueAsDouble("zmin") * 1_kpc;
	zmax = ffile->readKeyValueAsDouble("zmax") * 1_kpc;
	rmax = sqrt(xmax*xmax + ymax*ymax);
	
	dimx = ffile->readKeyValueAsInt("dimx");
	dimy = ffile->readKeyValueAsInt("dimy");
	dimz = ffile->readKeyValueAsInt("dimz");
   
	QLength deltax = (xmax - xmin) / (dimx - 1);
	QLength deltay = (ymax - ymin) / (dimy - 1);
	QLength deltaz = (zmax - zmin) / (dimz - 1);
 
	Vector3d origin(static_cast<double>(xmin), static_cast<double>(ymin), static_cast<double>(zmin));
	Vector3d spacing(static_cast<double>(deltax), static_cast<double>(deltay), static_cast<double>(deltaz));
	
	for (int i = 0; i < dimE; ++i) {
		grid.push_back(
			std::make_unique<ScalarGridQPDensityPerEnergy>(
				ScalarGridQPDensityPerEnergy(origin,
					dimx, dimy, dimz, spacing)));
	}

}
	
void Dragon3D::readDensity3D() {
    
	int hduIndex = 2;  
	int hduActual = 0;
	int hdu_type = 0;
	int firstElement = 1;
    
	auto vecSize = dimx * dimy * dimz;
	long nElements = energyRange.size() * vecSize;
	constexpr double fluxToDensity = static_cast<double>(4_pi/(c_light*1_GeV));
	
    	auto hduNumber = ffile->getNumberOfHDUs();
	while (hduActual < hduNumber) {
		ffile->moveToHDU(hduIndex); // Move to the next HDU (the first HDU = 1)

		if (ffile->getHDUType() != IMAGE_HDU) {
			std::cerr << "HDU is not an image!" << std::endl;
			hduIndex++;
			continue;
		}

	  	hduActual = ffile->getCurrentHDUNumber();	
      
		int Z = ffile->readKeyValueAsInt("Z_");
		int A = ffile->readKeyValueAsInt("A");
      
      		if (isPIDEnabled(PID(Z,A))) {
	
			std::cerr << "... reading species with Z = " << Z << " A = " << A << " at HDU = " << hduActual << std::endl;

			std::vector<float> rawData = ffile->readImageAsFloat(firstElement, nElements);
			
			std::size_t iE, ix, iy, iz; std::div_t dv;
			for (auto it = rawData.begin(); it != rawData.end(); ++it ) {
				std::size_t index = it - rawData.begin();

				dv = std::div(index, dimE);
				iE = dv.rem;
				dv = std::div(dv.quot, dimx);
				ix = dv.rem;
				dv = std::div(dv.quot, dimy);
				iy = dv.rem;
				dv = std::div(dv.quot, dimz);
				iz = dv.rem;

				(*it) *= fluxToDensity;

				grid[iE]->addValue(ix, iy, iz, static_cast<QPDensityPerEnergy>(*it));
			}
		}
		hduIndex++;
    }
}

} // namespace cosmicrays
} // namespace hermes

#endif // HERMES_HAVE_CFITSIO

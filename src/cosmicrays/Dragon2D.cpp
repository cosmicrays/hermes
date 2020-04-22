#ifdef HERMES_HAVE_CFITSIO

#include "hermes/Common.h"
#include "hermes/cosmicrays/Dragon2D.h"

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

Dragon2D::Dragon2D(
		const std::string& filename_, const PID& pid_)
    : CosmicRayDensity(pid_), filename(filename_) {
	readFile();
}

Dragon2D::Dragon2D(const PID& pid_) :
	CosmicRayDensity(pid_),
	filename(getDataPath(DEFAULT_CR_FILE)) {
	readFile();
}

Dragon2D::Dragon2D(const std::vector<PID> &pids_) :
	CosmicRayDensity(pids_),
	filename(getDataPath(DEFAULT_CR_FILE)) {
	readFile();
}	

Dragon2D::Dragon2D(
		const std::string& filename_, const std::vector<PID> &pids_) :
	CosmicRayDensity(pids_),
	filename(filename_) {
	readFile();
}

void Dragon2D::readFile() {
	ffile = std::make_unique<FITSFile>(FITSFile(filename)); 
  
	ffile->openFile(FITS::READ);
	ffile->moveToHDU(1);
	
	// read header
	readEnergyAxis();

	readSpatialGrid2D();
	readDensity2D();
}

QPDensityPerEnergy Dragon2D::getDensityPerEnergy(
		const QEnergy &E_, const Vector3QLength& pos_) const {
    	return getDensityPerEnergy(static_cast<int>(energyIndex.at(E_)), pos_);
}

QPDensityPerEnergy Dragon2D::getDensityPerEnergy(
		int iE_, const Vector3QLength& pos_) const {
	if (pos_.z < zmin || pos_.z > zmax)
		return QPDensityPerEnergy(0);
	
	QLength rho = sqrt(pos_.x*pos_.x + pos_.y*pos_.y);
	if (rho > rmax)
		return QPDensityPerEnergy(0);

	auto pos = Vector3QLength(rho, pos_.z, 0);
    	return (grid[iE_])->interpolate(pos);
}

void Dragon2D::readEnergyAxis() {
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

void Dragon2D::readSpatialGrid2D() {

	rmin = ffile->readKeyValueAsDouble("rmin") * 1_kpc;
	rmax = ffile->readKeyValueAsDouble("rmax") * 1_kpc;
	zmin = ffile->readKeyValueAsDouble("zmin") * 1_kpc;
	zmax = ffile->readKeyValueAsDouble("zmax") * 1_kpc;
	
	dimr = ffile->readKeyValueAsInt("dimr");
	dimz = ffile->readKeyValueAsInt("dimz");
	
	QLength deltar = (rmax - rmin) / (dimr - 1);
	QLength deltaz = (zmax - zmin) / (dimz - 1);
 
	//Vector3d origin(-1*rmax.getValue(), -1*rmax.getValue(), zmin.getValue());
	Vector3d origin(-1*static_cast<double>(rmax), static_cast<double>(zmin), 0);
	Vector3d spacing(static_cast<double>(deltar), static_cast<double>(deltaz), 0);
	
	for (int i = 0; i < dimE; ++i) {
		grid.push_back(
			std::make_unique<ScalarGrid2DQPDensityPerEnergy>(
				ScalarGrid2DQPDensityPerEnergy(origin,
					dimr, dimz, spacing)));
	}

}
 
std::size_t Dragon2D::calcArrayIndex2D(
	std::size_t iE, std::size_t ir, std::size_t iz) {
	return (iz*dimr + ir)*dimE + iE;
}
	
 
void Dragon2D::readDensity2D() {
    
	int hduIndex = 2;  
	int hduActual = 0;
	int hdu_type = 0;
	unsigned long firstElement = 1;
    
	auto vecSize = dimr * dimz;
	unsigned long nElements = energyRange.size() * vecSize;
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
			for (std::size_t iE = 0; iE < dimE; ++iE) {
				for (std::size_t ir = 0; ir < dimr; ++ir) {
					for (std::size_t iz = 0; iz < dimz; ++iz) {
						grid[iE]->addValue(ir, iz,
								fluxToDensity * rawData[calcArrayIndex2D(iE, ir, iz)]);
					}
				}
			}
		}
		hduIndex++;
    }
}

} // namespace cosmicrays
} // namespace hermes

#endif // HERMES_HAVE_CFITSIO

#ifdef HERMES_HAVE_CFITSIO

#include "hermes/cosmicRayDensity/DragonCRDensity.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace hermes {

DragonCRDensity::DragonCRDensity(const std::string& filename_, const PID& pid_)
    : CosmicRayDensity(), filename(filename_), fileType(DragonFileType::_3D) {
	enablePID(pid_);
	readFile();
}

DragonCRDensity::DragonCRDensity(const std::string& filename_, const PID& pid_, DragonFileType type_)
    : CosmicRayDensity(), filename(filename_), fileType(type_) {
	enablePID(pid_);
	readFile();
}

void DragonCRDensity::enablePID(const PID &pid_) {
	listOfPIDs.insert(pid_.getID());
}
	
void DragonCRDensity::disablePID(const PID &pid_) {
	listOfPIDs.erase(listOfPIDs.find(pid_.getID()));
}

bool DragonCRDensity::isPIDEnabled(const PID &pid_) const {
	return (listOfPIDs.count(pid_.getID()) > 0);
}

void DragonCRDensity::readFile() {
	ffile = std::make_unique<FITSFile>(FITSFile(filename)); 
  
	ffile->openFile(FITS::READ);
	ffile->moveToHDU(1);
	
	// read header
	readEnergyAxis();
	
	if (fileType == DragonFileType::_2D)
		readSpatialGrid2D();
	else
		readSpatialGrid3D();
	
	// read density grid
	if (fileType == DragonFileType::_2D)
		readDensity2D();
	else
		readDensity3D();
}

QPDensityPerEnergy DragonCRDensity::getDensityPerEnergy(
		const QEnergy &E_, const Vector3QLength& pos_) const {
    	return (grid[energyIndex.at(E_)])->interpolate(pos_);
}

QPDensityPerEnergy DragonCRDensity::getDensityPerEnergy(
		int iE_, const Vector3QLength& pos_) const {
    	return (grid[iE_])->interpolate(pos_);
}

void DragonCRDensity::readEnergyAxis() {
    	QEnergy E;
	double Ekmin = ffile->readKeyValueAsDouble("Ekmin");
	dimE = ffile->readKeyValueAsInt("dimE");
	energyScaleFactor = ffile->readKeyValueAsDouble("Ekin_fac");
	scaleFactorFlag = true;

	// input files are in GeV	
	for (int i = 0; i < dimE; ++i) {
		E = 1_GeV * std::exp(std::log(Ekmin) +
			static_cast<double>(i) * std::log(energyScaleFactor));
    		energyRange.push_back(E);
		energyIndex[E] = i;
	}
}

void DragonCRDensity::readSpatialGrid2D() {

	QLength rmin = ffile->readKeyValueAsDouble("rmin") * 1_kpc;
	QLength rmax = ffile->readKeyValueAsDouble("rmax") * 1_kpc;
	QLength zmin = ffile->readKeyValueAsDouble("zmin") * 1_kpc;
	QLength zmax = ffile->readKeyValueAsDouble("zmax") * 1_kpc;
	
	dimr = ffile->readKeyValueAsInt("dimr");
	dimz = ffile->readKeyValueAsInt("dimz");
	
	QLength deltar = (rmax - rmin) / (dimr - 1);
	QLength deltaz = (zmax - zmin) / (dimz - 1);
 
	Vector3d origin(-1*rmax.getValue(), -1*rmax.getValue(), zmin.getValue());
	Vector3d spacing(deltar.getValue(), deltar.getValue(), deltaz.getValue());
	
	for (int i = 0; i < dimE; ++i) {
		grid.push_back(
			std::make_unique<ScalarGridQPDensityPerEnergy>(
				ScalarGridQPDensityPerEnergy(origin,
					2*dimr, 2*dimr, dimz, spacing)));
	}

}
 

void DragonCRDensity::readSpatialGrid3D() {

	QLength xmin = ffile->readKeyValueAsDouble("xmin") * 1_kpc;
	QLength xmax = ffile->readKeyValueAsDouble("xmax") * 1_kpc;
	QLength ymin = ffile->readKeyValueAsDouble("ymin") * 1_kpc;
	QLength ymax = ffile->readKeyValueAsDouble("ymax") * 1_kpc;
	QLength zmin = ffile->readKeyValueAsDouble("zmin") * 1_kpc;
	QLength zmax = ffile->readKeyValueAsDouble("zmax") * 1_kpc;
	
	dimx = ffile->readKeyValueAsInt("dimx");
	dimy = ffile->readKeyValueAsInt("dimy");
	dimz = ffile->readKeyValueAsInt("dimz");
   
	QLength deltax = (xmax - xmin) / (dimx - 1);
	QLength deltay = (ymax - ymin) / (dimy - 1);
	QLength deltaz = (zmax - zmin) / (dimz - 1);
 
	Vector3d origin(xmin.getValue(), ymin.getValue(), zmin.getValue());
	Vector3d spacing(deltax.getValue(), deltay.getValue(), deltaz.getValue());
	
	for (int i = 0; i < dimE; ++i) {
		grid.push_back(
			std::make_unique<ScalarGridQPDensityPerEnergy>(
				ScalarGridQPDensityPerEnergy(origin,
					dimx, dimy, dimz, spacing)));
	}

}
	
std::size_t DragonCRDensity::calcArrayIndex2D(
	std::size_t iE, std::size_t ir, std::size_t iz) {
	return (iz*dimr + ir)*dimE + iE;
}
	
 
void DragonCRDensity::readDensity2D() {
    
	int hduIndex = 2;  
	int hduActual = 0;
	int hdu_type = 0;
	int firstElement = 1;
    
	int Z = 0, A = 0;
	
	auto vecSize = dimr * dimz;
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
      
		Z = ffile->readKeyValueAsInt("Z_");
		A = ffile->readKeyValueAsInt("A");
     
      		if (isPIDEnabled(PID(Z,A))) {
	
			std::cerr << "... reading species with Z = " << Z << " A = " << A << " at HDU = " << hduActual << std::endl;

			std::vector<float> rawData = ffile->readImageAsFloat(firstElement, nElements);
	
			double dist;
			std::size_t ix_c, iy_c, ir_back, ir_front;
			QPDensityPerEnergy frontValue, backValue, interpolatedValue;

			for (std::size_t iE = 0; iE < dimE; ++iE) {
				for (std::size_t ix = 0; ix < 2*dimr; ++ix) {
					for (std::size_t iy = 0; iy < 2*dimr; ++iy) {
						
						ix_c = ix-dimr+2; iy_c = iy-dimr+2;
						dist = std::sqrt(ix_c*ix_c + iy_c*iy_c);
						
						// determine discrete points
						ir_back = static_cast<int>(std::trunc(dist));
						ir_front = ir_back + 1;

						if (ir_front >= dimr) continue;

						for (std::size_t iz = 0; iz < dimz; ++iz) {
							frontValue = fluxToDensity * rawData[calcArrayIndex2D(iE, ir_back, iz)];
							backValue = fluxToDensity * rawData[calcArrayIndex2D(iE, ir_front, iz)];
	
							// two-point linear interpolation (between two "rings")
							interpolatedValue = (frontValue - backValue) * (dist - ir_back)
										+ backValue;
							grid[iE]->addValue(ix, iy, iz, interpolatedValue);
						}
					}
				}
			}
		}
		hduIndex++;
    }
}

void DragonCRDensity::readDensity3D() {
    
	int hduIndex = 2;  
	int hduActual = 0;
	int hdu_type = 0;
	int firstElement = 1;
    
	int Z = 0, A = 0;
	
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
      
		Z = ffile->readKeyValueAsInt("Z_");
		A = ffile->readKeyValueAsInt("A");
      
      		if (isPIDEnabled(PID(Z,A))) {
	
			std::cerr << "... reading species with Z = " << Z << " A = " << A << " at HDU = " << hduActual << std::endl;

			std::vector<float> rawData = ffile->readImageAsFloat(firstElement, nElements);
			
			std::size_t index, iE, ix, iy, iz; std::div_t dv;
			for (auto it = rawData.begin(); it != rawData.end(); ++it ) {
				index = it - rawData.begin();

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

} // namespace hermes

#endif // HERMES_HAVE_CFITSIO

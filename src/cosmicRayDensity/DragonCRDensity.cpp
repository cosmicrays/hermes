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

Dragon2DCRDensity::Dragon2DCRDensity(
		const std::string& filename_, const PID& pid_)
    : CosmicRayDensity(true), filename(filename_) {
	enablePID(pid_);
	readFile();
}

Dragon2DCRDensity::Dragon2DCRDensity(
		const std::string& filename_, const std::vector<PID> &pids_)
    : CosmicRayDensity(true), filename(filename_) {
	for(auto const& p: pids_)
		enablePID(p);
	readFile();
}

void Dragon2DCRDensity::enablePID(const PID &pid_) {
	listOfPIDs.insert(pid_.getID());
}
	
void Dragon2DCRDensity::disablePID(const PID &pid_) {
	listOfPIDs.erase(listOfPIDs.find(pid_.getID()));
}

bool Dragon2DCRDensity::isPIDEnabled(const PID &pid_) const {
	return (listOfPIDs.count(pid_.getID()) > 0);
}

void Dragon2DCRDensity::readFile() {
	ffile = std::make_unique<FITSFile>(FITSFile(filename)); 
  
	ffile->openFile(FITS::READ);
	ffile->moveToHDU(1);
	
	// read header
	readEnergyAxis();

	readSpatialGrid2D();
	readDensity2D();
}

QPDensityPerEnergy Dragon2DCRDensity::getDensityPerEnergy(
		const QEnergy &E_, const Vector3QLength& pos_) const {
    	return getDensityPerEnergy(static_cast<int>(energyIndex.at(E_)), pos_);
}

QPDensityPerEnergy Dragon2DCRDensity::getDensityPerEnergy(
		int iE_, const Vector3QLength& pos_) const {
	if (pos_.z < zmin || pos_.z > zmax)
		return QPDensityPerEnergy(0);
	
	QLength rho = sqrt(pos_.x*pos_.x + pos_.y*pos_.y);
	if (rho > rmax)
		return QPDensityPerEnergy(0);

	auto pos = Vector3QLength(rho, pos_.z, 0);
    	return (grid[iE_])->interpolate(pos);
}

void Dragon2DCRDensity::readEnergyAxis() {
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

void Dragon2DCRDensity::readSpatialGrid2D() {

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
 
std::size_t Dragon2DCRDensity::calcArrayIndex2D(
	std::size_t iE, std::size_t ir, std::size_t iz) {
	return (iz*dimr + ir)*dimE + iE;
}
	
 
void Dragon2DCRDensity::readDensity2D() {
    
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

// Dragon3DCRDensity

Dragon3DCRDensity::Dragon3DCRDensity(
		const std::string& filename_, const PID& pid_)
    : CosmicRayDensity(true), filename(filename_) {
	enablePID(pid_);
	readFile();
}

Dragon3DCRDensity::Dragon3DCRDensity(
		const std::string& filename_, const std::vector<PID> &pids_)
    : CosmicRayDensity(true), filename(filename_) {
	for(auto const& p: pids_)
		enablePID(p);
	readFile();
}

void Dragon3DCRDensity::enablePID(const PID &pid_) {
	listOfPIDs.insert(pid_.getID());
}
	
void Dragon3DCRDensity::disablePID(const PID &pid_) {
	listOfPIDs.erase(listOfPIDs.find(pid_.getID()));
}

bool Dragon3DCRDensity::isPIDEnabled(const PID &pid_) const {
	return (listOfPIDs.count(pid_.getID()) > 0);
}

void Dragon3DCRDensity::readFile() {
	ffile = std::make_unique<FITSFile>(FITSFile(filename)); 
  
	ffile->openFile(FITS::READ);
	ffile->moveToHDU(1);
	
	// read header
	readEnergyAxis();
	
	readSpatialGrid3D();
	readDensity3D();
}

QPDensityPerEnergy Dragon3DCRDensity::getDensityPerEnergy(
		const QEnergy &E_, const Vector3QLength& pos_) const {
    	return getDensityPerEnergy(static_cast<int>(energyIndex.at(E_)), pos_);
}

QPDensityPerEnergy Dragon3DCRDensity::getDensityPerEnergy(
		int iE_, const Vector3QLength& pos_) const {
	if (pos_.z < zmin || pos_.z > zmax)
		return QPDensityPerEnergy(0);
	
	QLength rho = sqrt(pos_.x*pos_.x + pos_.y*pos_.y);
	if (rho > rmax)
		return QPDensityPerEnergy(0);

    	return (grid[iE_])->interpolate(pos_);
}

void Dragon3DCRDensity::readEnergyAxis() {
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

void Dragon3DCRDensity::readSpatialGrid3D() {

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
	
void Dragon3DCRDensity::readDensity3D() {
    
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

} // namespace hermes

#endif // HERMES_HAVE_CFITSIO

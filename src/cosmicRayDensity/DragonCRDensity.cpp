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
    : filename(filename_), pid(pid_) {
    readHeaderFromFITS();
    readDensityFromFITS();
}

void DragonCRDensity::readHeaderFromFITS() {
	ffile = std::make_unique<FITSFile>(FITSFile(filename)); 
  
	ffile->openFile(FITS::READ);
	ffile->moveToHDU(1);

	readEnergyAxis();
	readSpatialGrid();
}

QPDensityPerEnergy DragonCRDensity::getDensityPerEnergy(
		const QEnergy &E_, const Vector3QLength& pos_) const {
    	return (grid[getIndexOfE(E_)])->interpolate(pos_);
}



void DragonCRDensity::readEnergyAxis() {
    
	double Ekmin = ffile->readKeyValueAsDouble("Ekmin");
	double Ekfact = ffile->readKeyValueAsDouble("Ekin_fac");
	int nE = ffile->readKeyValueAsInt("dimE");

	// input files are in GeV	
	for (int i = 0; i < nE; ++i) {
    		energyRange.push_back(std::exp(std::log(Ekmin) +
			static_cast<double>(i) * std::log(Ekfact)) * 1_GeV); 
	}
}

void DragonCRDensity::readSpatialGrid() {

	QLength xmax = ffile->readKeyValueAsDouble("xmax") * 1_kpc;
	QLength xmin = ffile->readKeyValueAsDouble("xmin") * 1_kpc;
	QLength ymax = ffile->readKeyValueAsDouble("ymax") * 1_kpc;
	QLength ymin = ffile->readKeyValueAsDouble("ymin") * 1_kpc;
	QLength zmin = ffile->readKeyValueAsDouble("zmin") * 1_kpc;
	QLength zmax = ffile->readKeyValueAsDouble("zmax") * 1_kpc;
	//int dimr = ffile->readKeyValueAsInt("dimr");
	dimx = ffile->readKeyValueAsInt("dimx");
	dimy = ffile->readKeyValueAsInt("dimy");
	dimz = ffile->readKeyValueAsInt("dimz");

	// ?!
    	/*if (ffile->getStatus()) { 
		std::cout << "Setting up 2D grid\n"; 
		ny = 0;
		do3D = false;
	} else {
		std::cout << "Setting up 3D grid\n";
		do3D = true;
	}*/
   
	QLength deltax = (xmax - xmin) / (dimx - 1);
	QLength deltay = (ymax - ymin) / (dimy - 1);
	QLength deltaz = (zmax - zmin) / (dimz - 1);
 
	Vector3d origin(xmin.getValue(), ymin.getValue(), zmin.getValue());
	Vector3d spacing(deltax.getValue(), deltay.getValue(), deltaz.getValue());
	
	for (int i = 0; i < energyRange.size(); ++i) {
		grid.push_back(
			std::make_unique<ScalarGridQPDensityPerEnergy>(
				ScalarGridQPDensityPerEnergy(origin,
					dimx, dimy, dimz, spacing)));
	}

}
 

void DragonCRDensity::readDensityFromFITS() {
    
	int anynul = -1;
	int status = 0;
	int hduIndex = 2;  
	int hduActual = 0;
	int hdu_type = 0;
	int firstElement = 1;
	std::vector<float>::const_iterator start, end;
    
	int Z = 0, A = 0;
	
	auto vecSize = dimx * dimy * dimz;
	long nElements = energyRange.size() * vecSize;
	
    	auto hduNumber = ffile->getNumberOfHDUs();
	while (hduActual < hduNumber) {
		ffile->moveToHDU(hduIndex); // Move to the next HDU (the first HDU = 1)
		if (ffile->getHDUType() != IMAGE_HDU)
			std::cerr << "Not an image!" << std::endl;

	  	hduActual = ffile->getCurrentHDUNumber();	
      
		Z = ffile->readKeyValueAsInt("Z_");
		A = ffile->readKeyValueAsInt("A");
      
      		if (Z == pid.atomicNr() && A == pid.massNr()) {
		 	isPresent = true;
	
			std::cout << "... reading species with Z = " << Z << " A = " << A << " at HDU = " << hduActual << std::endl;

			std::vector<float> origVec = ffile->readImageAsFloat(firstElement, nElements);
			
			for (int i = 0; i < energyRange.size(); ++i) {
				start = origVec.begin() + i * vecSize;
				end = origVec.begin() + (i+1) * vecSize;
				auto densityPerE = std::make_unique<std::vector<QPDensityPerEnergy> >(vecSize);
				std::transform(start, end, densityPerE->begin(),
					[](const float i) { return static_cast<QPDensityPerEnergy>(i); });
				grid[i]->addVector(std::move(densityPerE));
			}
		}
		hduIndex++;
    }

    std::cerr << "Value: " << grid[3]->getValue(10, 1, 1) << std::endl;
}

} // namespace hermes

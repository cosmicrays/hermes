#include "hermes/cosmicRayDensity/DragonCRDensity.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "fitsio.h"

namespace hermes {

DragonCRDensity::DragonCRDensity(const std::string& filename_, const PID& pid_)
    : filename(filename_), pid(pid_) {
    readHeaderFromFITS();
    readDensityFromFITS();
}

QPDensityPerEnergy DragonCRDensity::getDensityPerEnergy(const Vector3QLength& pos, const QEnergy& E_) const {
    	// HERE WE SHOULD PUT SOME KIND OF INTERPOLATION
}

void DragonCRDensity::readHeaderFromFITS() {
	ffile = std::make_unique<FITSFile>(FITSFile(filename)); 
  
	ffile->openFile(read);
	ffile->moveToHDU(1);

	readEnergyAxis();
	readSpatialGrid();
}

void DragonCRDensity::readEnergyAxis() {
    
	double Ekmin = ffile->readKeyValueAsDouble("Ekmin");
	double Ekfact = ffile->readKeyValueAsDouble("Ekin_fac");
	int nE = ffile->readKeyValueAsInt("dimE");
	
	for (int i = 0; i < nE; ++i) {
    		energyRange.push_back(std::exp(std::log(Ekmin) +
					static_cast<double>(i) * std::log(Ekfact)) * 1_eV); 
	}
}

void DragonCRDensity::readSpatialGrid() {
   	int nr, ny, nz;

	double rmax = ffile->readKeyValueAsDouble("rmax");
	double zmin = ffile->readKeyValueAsDouble("zmin");
	double zmax = ffile->readKeyValueAsDouble("zmax");
	int dimr = ffile->readKeyValueAsInt("dimr");
	int dimz = ffile->readKeyValueAsInt("dimz");
	int dimy = ffile->readKeyValueAsInt("dimy");

	// ?!
    	if (ffile->getStatus()) { 
		std::cout << "Setting up 2D grid\n"; 
		ny = 0;
		do3D = false;
	} else {
		std::cout << "Setting up 3D grid\n";
		do3D = true;
	}
    
    auto deltar = rmax / (double)(nr - 1);
    auto deltay = (!ny) ? 0.0 : rmax / double(ny - 1);
    auto deltaz = (zmax - zmin) / (double)(nz - 1);
  
    for (int i = 0; i < nr; ++i)
      r.push_back((double)i * deltar);
    for (int i = 0; i < ny; ++i)
      y.push_back((double)i * deltar);
    for (int i = 0; i < nz; ++i)
      z.push_back(zmin + (double)i * deltaz);
  }
 

void DragonCRDensity::readDensityFromFITS() {
    
	int counterInd = 0;
	int anynul = -1;
	int status = 0;
	int hduIndex = 2;  
	int hduActual = 0;
	int hdu_type = 0;
	int firstElement = 1;
    
	int Z = 0, A = 0;

	long nElements = r.size() * z.size() * y.size();
	//nElements *= (do3D) ? y.size() : 1;
    
	std::unique_ptr<std::vector<float> > densityForE;
	
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
	
			densityForE = ffile->readImageAsFloat(firstElement, nElements);
	
			counterInd = 0;
	
			// HERE WE ADD readingCrDensityVec TO A 2D or 3D grid, consider that there could be MORE THAN 1 HDUs
      		}
		hduIndex++;
    }
}

} // namespace hermes

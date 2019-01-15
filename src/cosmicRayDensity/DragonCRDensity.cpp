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

class PID {
public:
  PID(int Z_, int A_) : Z(Z_), A(A_) {
    assert(A_ >= 0);
    id = Z * 100 + A;
  }

  virtual ~PID() {
  }

  bool isNucleus() {
    return (A > 0);
  }

  int atomicNr() const {
    return Z;
  }

  int massNr() const {
    return A;
  }
  
protected:
  int Z;
  int A;
  int id;
};

static const PID Electron(-1, 0);
static const PID Positron(1, 0);
static const PID Proton(1, 1);
static const PID Helium(2, 4);

DragonCRDensity(const std::string& filename_, const PID& pid_)
    : filename(filename_), pid(pid_) {
    readHeaderFromFITS();
    readDensityFromFITS();
}

QPDensityPerEnergy DragonCRDensity::getDensityPerEnergy(const Vector3QLength& pos, const QEnergy& E_) const {
    	// HERE WE SHOULD PUT SOME KIND OF INTERPOLATION
	constexpr int alpha = 3;
	auto Phi0 = 0.1 / (1_GeV * 1_cm*1_cm * 1_s * c_light) * 4_pi; 
	auto E0 = 1_GeV;
	auto E_cutoff = 5_TeV;

	QNumber profile = exp(-1.*fabs(pos.getZ())/1_kpc);
	QPDensityPerEnergy spectrum = Phi0 * pow<-1*alpha>(E_ / E0) * exp(-E_ / E_cutoff);

	return profile * spectrum;
}

} // namespace hermes


/*
class DragonModel : public CosmicRayModel {
public:

  void readEnergyAxis(fitsfile *infile, FILE *fp) {
    double Ekmin, Ekfact;
    int nE, status;
    if (fits_read_key(infile, TDOUBLE, "Ekmin",    &Ekmin,  NULL, &status))
      fits_report_error(fp, status);
    if (fits_read_key(infile, TDOUBLE, "Ekin_fac", &Ekfact, NULL, &status))
      fits_report_error(fp, status);
    if (fits_read_key(infile, TINT,    "dimE",     &nE,     NULL, &status))
      fits_report_error(fp, status);
    for (int i = 0; i < nE; ++i)
      E.push_back(std::exp(std::log(Ekmin) + (double)i * std::log(Ekfact))); 
  }

  void readSpatialGrid(fitsfile *infile, FILE *fp) {
    double rmax, zmin, zmax;
    int nr, ny, nz, status;
    if (fits_read_key(infile, TDOUBLE, "rmax",     &rmax,   NULL, &status))
      fits_report_error(fp, status);
    if (fits_read_key(infile, TDOUBLE, "zmin",     &zmin,   NULL, &status))
      fits_report_error(fp, status);
    if (fits_read_key(infile, TDOUBLE, "zmax",     &zmax,   NULL, &status))
      fits_report_error(fp, status);
    if (fits_read_key(infile, TINT,    "dimr",     &nr,     NULL, &status))
      fits_report_error(fp, status);
    if (fits_read_key(infile, TINT,    "dimz",     &nz,     NULL, &status))
      fits_report_error(fp, status);
    if (fits_read_key(infile, TINT,    "dimy",     &ny,     NULL, &status))
      fits_report_error(fp, status);
    if (status) { 
      std::cout << "Setting up 2D grid\n"; 
      ny = 0;
      do3D = false;
    }
    else {
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
 
  void readHeaderFromFITS() {
    FILE *fp;
    fp = fopen("cfitsio.log","w+");

    fitsfile * infile = NULL;
  
    int status = 0, hdutype = 0;

    if (fits_open_file(&infile, filename.c_str(), READONLY, &status)) 
      fits_report_error(fp, status);
    
    if (fits_movabs_hdu(infile, 1, &hdutype, &status)) 
      fits_report_error(fp, status);

    readEnergyAxis(infile, fp);
    readSpatialGrid(infile, fp);
    
    if (fits_close_file(infile, &status))
      fits_report_error(fp, status);
       
    fclose(fp);
  }

  void readDensityFromFITS() {
  
    fitsfile * infile = NULL;
    float * nullval  = NULL;
    
    int counterInd = 0;
    int anynul = -1;
    int status = 0;
    int hdu_index = 2;  
    int hdu_actual = 0;
    int hdu_num = 0;  
    int hdu_type = 0;
    int fpixel = 1;
    
    int Z = 0, A = 0;

    long nelements = E.size() * r.size() * z.size();
    nelements *= (do3D) ? y.size() : 1;
    
    density = std::vector<double>(nelements, 0.);
    
    float * readingCrDensityVec = new float[nelements]();
    
    if (fits_open_file(&infile, filename.c_str(), READONLY, &status)) 
      fits_report_error(stderr, status);
    
    if (fits_get_num_hdus(infile, &hdu_num, &status))
      fits_report_error(stderr, status);
    
    while (hdu_actual < hdu_num) {
      
      if (fits_movabs_hdu(infile, hdu_index, &hdu_type, &status)) 
	fits_report_error(stderr, status); // Move to the next HDU (the first HDU = 1)
      
      if (hdu_type != IMAGE_HDU)
	std::cerr << "Not an image!\n";
      
      fits_get_hdu_num(infile, &hdu_actual);
      
      if (fits_read_key(infile, TINT, "Z_", &Z, NULL, &status) )
	fits_report_error(stderr, status);
      
      if ( fits_read_key(infile, TINT, "A",  &A, NULL, &status) ) 
	fits_report_error(stderr, status);
      
      if (Z == pid.atomicNr() && A == pid.massNr()) {
	
 	isPresent = true;
	
	std::cout << "... reading species with Z = " << Z << " A = " << A << " at HDU = " << hdu_actual << std::endl;
	
	if (fits_read_img(infile, TFLOAT, fpixel, nelements, nullval, readingCrDensityVec, &anynul, &status)) 
	  fits_report_error(stderr, status);
	
	counterInd = 0;
	
	// HERE WE ADD readingCrDensityVec TO A 2D or 3D grid, consider that there could be MORE THAN 1 HDUs
      }
      
      hdu_index++;
    }
    
    delete [] readingCrDensityVec;
    readingCrDensityVec = NULL;
    
    if (fits_close_file(infile, &status))
      fits_report_error(stderr, status);
  }
  
protected:
  bool do3D = false;
  bool isPresent = false;
  std::vector<double> r, y, z;
  std::vector<double> E;
  std::vector<double> density;
  std::string filename;
  PID pid;
};
*/

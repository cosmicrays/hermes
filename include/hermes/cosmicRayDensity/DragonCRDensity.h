#ifndef HERMES_DRAGONCRDENSITY_H
#define HERMES_DRAGONCRDENSITY_H

#include "hermes/cosmicRayDensity/CosmicRayDensity.h"
#include "hermes/FITSWrapper.h"
#include <cassert>
#include <memory>

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

class DragonCRDensity: public CosmicRayDensity {
private:
  	std::string filename;
	std::unique_ptr<FITSFile> ffile;
	PID pid;

	void readHeaderFromFITS();
	void readDensityFromFITS();
	void readEnergyAxis();
	void readSpatialGrid();
  
	bool do3D = false;
	bool isPresent = false;
	int dimx, dimy, dimz;
	std::vector<std::unique_ptr<ScalarGridQPDensityPerEnergy> > grid;
public:
	DragonCRDensity();
	DragonCRDensity(const std::string& filename_, const PID& pid_);
	DragonCRDensity(QEnergy minE, QEnergy maxE, int steps);
	QPDensityPerEnergy getDensityPerEnergy(const QEnergy& E_, const Vector3QLength& pos_) const;
};

} // namespace hermes

#endif // HERMES_DRAGONCRDENSITY_H

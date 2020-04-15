#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_DRAGON2D_H
#define HERMES_DRAGON2D_H

#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/FITSWrapper.h"

#include <memory>
#include <map>
#include <set>

namespace hermes { namespace cosmicrays {

class Dragon2D: public CosmicRayDensity {
private:
  	std::string filename;
	std::unique_ptr<FITSFile> ffile;

	void readFile();
	void readEnergyAxis();
	void readSpatialGrid2D();
	void readDensity2D();
	std::size_t calcArrayIndex2D(
		std::size_t iE, std::size_t ir, std::size_t iz);

	QLength rmin, rmax, zmin, zmax;
	int dimE;
	int dimz, dimr;
	std::vector<std::unique_ptr<ScalarGrid2DQPDensityPerEnergy> > grid;
	
	//TODO: implement as std::unordered_map
	std::map<QEnergy, std::size_t> energyIndex;
public:
	Dragon2D(const PID& pid);
	Dragon2D(const std::vector<PID> &pids);
	Dragon2D(const std::string &filename,
			const PID &pid_);
	Dragon2D(const std::string &filename,
			const std::vector<PID> &pids);
	QPDensityPerEnergy getDensityPerEnergy(const QEnergy& E_,
			const Vector3QLength& pos_) const override;
	QPDensityPerEnergy getDensityPerEnergy(int iE_,
			const Vector3QLength& pos_) const;
};

} // namespace cosmicrays
} // namespace hermes

#endif // HERMES_DRAGON2D_H

#endif // HERMES_HAVE_CFITSIO

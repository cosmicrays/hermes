#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_DRAGON3D_H
#define HERMES_DRAGON3D_H

#include <array>
#include <map>
#include <memory>
#include <set>

#include "hermes/FITSWrapper.h"
#include "hermes/cosmicrays/CosmicRayDensity.h"

namespace hermes { namespace cosmicrays {
/**
 * \addtogroup CosmicRays
 * @{
 */

class Dragon3D : public CosmicRayDensity {
  private:
	std::string filename;
	std::unique_ptr<FITSFile> ffile;

	void readFile();
	void readEnergyAxis();
	void readSpatialGrid3D();
	void readDensity3D();
	std::size_t calcArrayIndex2D(std::size_t iE, std::size_t ir,
	                             std::size_t iz);

	QLength rmin, rmax, zmin, zmax;
	QLength xmin, xmax, ymin, ymax;
	int dimE;
	int dimx, dimy, dimz, dimr;
	std::vector<std::unique_ptr<ScalarGridQPDensityPerEnergy>> grid;

	// TODO: implement as std::unordered_map
	std::map<QEnergy, std::size_t> energyIndex;

  public:
	Dragon3D(const PID &pid);
	Dragon3D(const std::vector<PID> &pids);
	Dragon3D(const std::string &filename, const PID &pid_);
	Dragon3D(const std::string &filename, const std::vector<PID> &pids);
	QPDensityPerEnergy getDensityPerEnergy(
	    const QEnergy &E_, const Vector3QLength &pos_) const override;
	QPDensityPerEnergy getDensityPerEnergy(int iE_,
	                                       const Vector3QLength &pos_) const;
    std::array<QLength, 2> getXBoundaries() const;
    std::array<QLength, 2> getYBoundaries() const;
    std::array<QLength, 2> getZBoundaries() const;
};

/** @}*/
}}  // namespace hermes::cosmicrays

#endif  // HERMES_DRAGON3D_H

#endif  // HERMES_HAVE_CFITSIO

#ifdef HERMES_HAVE_HDF5

#ifndef HERMES_PICARD3D_H
#define HERMES_PICARD3D_H

#include <array>
#include <map>
#include <memory>
#include <set>

#include "hermes/FITSWrapper.h"
#include "hermes/Hdf5Reader.h"
#include "hermes/cosmicrays/CosmicRayDensity.h"

namespace hermes { namespace cosmicrays {
/**
 * \addtogroup CosmicRays
 * @{
 */

class Picard3D : public CosmicRayDensity {
  private:
	std::string cosmicRayFluxesDirectory;
	std::unique_ptr<Hdf5Reader> h5File;
	std::string h5Filename;

	std::string findFinalTimeStepDirectory();
	void readFile();
	void readEnergyAxis();
	void evaluateEnergyScaleFactor();
	void readSpatialGrid3D();
	void readDensity3D();
	std::size_t getArrayIndex3D(std::size_t xIndex, std::size_t yIndex,
	                            std::size_t zIndex) const;
	std::string getDatasetName(std::size_t energyIndex);

	QLength zMin, zMax;
	QLength xMin, xMax;
	QLength yMin, yMax;

	size_t numberOfEnergies{};
	size_t numberOfXValues{};
	size_t numberOfYValues{};
	size_t numberOfZValues{};

	std::vector<std::unique_ptr<ScalarGridQPDensityPerEnergy>> grid;

	// TODO: implement as std::unordered_map
	std::map<QEnergy, std::size_t> energyToIndex;

  public:
	/**
	 * Initializes Picard3D with the test cosmic ray fluxes. It's not
	 * recommended to use that for an actual simulation.
	 * @param particleID particle used for the simulation
	 */
	explicit Picard3D(const PID &particleID);
	/**
	 * Initializes Picard3D with the test cosmic ray fluxes. It's not
	 * recommended to use that for an actual simulation.
	 * @param particleIDs particles used for the simulation
	 */
	explicit Picard3D(const std::vector<PID> &particleIDs);
	/**
	 * @param cosmicRayFluxesDirectory directory that contains the simulation
	 *                                 results from Picard
	 * @param particleID particle used for the simulation
	 */
	Picard3D(std::string cosmicRayFluxesDirectory, const PID &particleID);
	/**
	 * @param cosmicRayFluxesDirectory directory that contains the simulation
	 *                                 results from Picard
	 * @param particleIDs particles used for the simulation
	 */
	Picard3D(std::string cosmicRayFluxesDirectory,
	         const std::vector<PID> &particleIDs);
	QPDensityPerEnergy getDensityPerEnergy(
	    const QEnergy &energy, const Vector3QLength &position) const override;
	QPDensityPerEnergy getDensityPerEnergy(
	    int energyIndex, const Vector3QLength &position) const;
	std::array<QLength, 2> getXBoundaries() const;
	std::array<QLength, 2> getYBoundaries() const;
	std::array<QLength, 2> getZBoundaries() const;
};

/** @}*/
}}  // namespace hermes::cosmicrays

#endif  // HERMES_PICARD3D_H

#endif  // HERMES_HAVE_HDF5

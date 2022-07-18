#ifdef HERMES_HAVE_HDF5

#include "hermes/cosmicrays/Picard3D.h"

#include <array>
#include <cassert>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "hermes/Common.h"

#define DEFAULT_CR_DIRECTORY "CosmicRays/Picard_testing"

namespace hermes { namespace cosmicrays {

Picard3D::Picard3D(std::string cosmicRayFluxesDirectory, const PID &particleID)
    : CosmicRayDensity(particleID),
      cosmicRayFluxesDirectory(std::move(cosmicRayFluxesDirectory)) {
	readFile();
}

Picard3D::Picard3D(const PID &particleID)
    : CosmicRayDensity(particleID),
      cosmicRayFluxesDirectory(getDataPath(DEFAULT_CR_DIRECTORY)) {
	readFile();
}

Picard3D::Picard3D(const std::vector<PID> &particleIDs)
    : CosmicRayDensity(particleIDs),
      cosmicRayFluxesDirectory(getDataPath(DEFAULT_CR_DIRECTORY)) {
	readFile();
}

Picard3D::Picard3D(std::string cosmicRayFluxesDirectory,
                   const std::vector<PID> &particleIds)
    : CosmicRayDensity(particleIds),
      cosmicRayFluxesDirectory(std::move(cosmicRayFluxesDirectory)) {
	readFile();
}

std::string Picard3D::findFinalTimeStepDirectory() {
	auto files = std::filesystem::directory_iterator(cosmicRayFluxesDirectory);
	std::string finalTimeStepDirectory;
	bool foundDirectory{false};
	for (const auto &file : files) {
		std::string filePath = file.path();
		bool isFinalTimeStep =
		    std::regex_match(filePath, std::regex(".*_tfinal"));
		if (isFinalTimeStep) {
			foundDirectory = true;
			finalTimeStepDirectory = filePath;
			break;
		}
	}
	if (!foundDirectory) {
		std::cerr << "hermes: error: Couldn't find the directory *_tfinal in "
		             "the specified cosmic ray fluxes directory '"
		          << cosmicRayFluxesDirectory << "'." << std::endl;
		std::exit(1);
	}
	return finalTimeStepDirectory;
}

void Picard3D::readFile() {
	std::string finalTimeStepDirectory = findFinalTimeStepDirectory();
	auto speciesFiles =
	    std::filesystem::directory_iterator(finalTimeStepDirectory);
	bool gotEnergyAxisAndSpatialGrid{false};
	for (const auto &speciesFile : speciesFiles) {
		const auto &path = speciesFile.path();
		bool isH5File = path.extension() == ".h5";
		bool isGammaRayFile = std::regex_match(std::string(path.filename()),
		                                       std::regex("^Emiss_.*h5"));
		if (!isH5File || isGammaRayFile) {
			continue;
		}
		h5File = std::make_unique<Hdf5Reader>(speciesFile.path());
		if (!gotEnergyAxisAndSpatialGrid) {
			readEnergyAxis();
			readSpatialGrid3D();
			gotEnergyAxisAndSpatialGrid = true;
		}
		h5Filename = speciesFile.path().filename();
		readDensity3D();
	}
}

void Picard3D::evaluateEnergyScaleFactor() {
	std::vector<double> energyScaleFactors;
	double evaluatedFactor;
	for (int i = 0; i < (numberOfEnergies - 1); ++i) {
		evaluatedFactor = static_cast<double>(energyRange[i + 1]) /
		                  static_cast<double>(energyRange[i]);
		energyScaleFactors.push_back(evaluatedFactor);
	}
	double tolerance = 1e-6;
	bool constantOverAllEnergies = std::all_of(
	    energyScaleFactors.begin(), energyScaleFactors.end(),
	    [&](double const &currentFactor) {
		    return std::abs(evaluatedFactor - currentFactor) < tolerance;
	    });
	if (!constantOverAllEnergies) {
		std::cerr << "hermes: error: Picard3D: Could not determine the energy "
		             "scale factor because it is not constant over all "
		             "energies. Energy grid is not supported.\n";
		std::exit(1);
	} else {
		energyScaleFactor = evaluatedFactor;
		setScaleFactor(true);
	}
}

void Picard3D::readEnergyAxis() {
	// TODO: Does hermes need the kinetic energy per nucleon or per nucleus?
	QEnergy energy;
	h5File->readAttributeFromDataGroup("Entries", numberOfEnergies);
	for (int energyIndex = 0; energyIndex < numberOfEnergies; ++energyIndex) {
		std::string datasetName(getDatasetName(energyIndex));
		double energyValue;
		h5File->readAttributeFromDatasetOfDataGroup(
		    datasetName, "kinetic energy per nucleon", energyValue);
		energy = 1_MeV * energyValue;
		energyRange.push_back(energy);
		energyToIndex[energy] = energyIndex;
	}
	evaluateEnergyScaleFactor();
}

void Picard3D::readSpatialGrid3D() {
	std::vector<float> xCenter, yCenter, zCenter;
	h5File->readAttributeFromDataGroup("xGridCentred", xCenter);
	h5File->readAttributeFromDataGroup("yGridCentred", yCenter);
	h5File->readAttributeFromDataGroup("zGridCentred", zCenter);

	xMin = xCenter[0] * 1_kpc;
	xMax = xCenter[xCenter.size() - 1] * 1_kpc;

	yMin = yCenter[0] * 1_kpc;
	yMax = yCenter[yCenter.size() - 1] * 1_kpc;

	zMin = zCenter[0] * 1_kpc;
	zMax = zCenter[zCenter.size() - 1] * 1_kpc;

	numberOfXValues = xCenter.size();
	numberOfYValues = yCenter.size();
	numberOfZValues = zCenter.size();

	QLength xSpacing = (xMax - xMin) / (numberOfXValues - 1);
	QLength ySpacing = (yMax - yMin) / (numberOfYValues - 1);
	QLength zSpacing = (zMax - zMin) / (numberOfZValues - 1);

	Vector3d spacing(static_cast<double>(xSpacing),
	                 static_cast<double>(ySpacing),
	                 static_cast<double>(zSpacing));

	Vector3d gridOrigin(static_cast<double>(xMin), static_cast<double>(yMin),
	                    static_cast<double>(zMin));

	Vector3d volumeOrigin = gridOrigin - spacing * 0.5;

	for (int energyIndex = 0; energyIndex < numberOfEnergies; ++energyIndex) {
		grid.push_back(std::make_unique<ScalarGridQPDensityPerEnergy>(
		    ScalarGridQPDensityPerEnergy(volumeOrigin, numberOfXValues,
		                                 numberOfYValues, numberOfZValues,
		                                 spacing)));
	}
}

std::size_t Picard3D::getArrayIndex3D(std::size_t xIndex, std::size_t yIndex,
                                      std::size_t zIndex) const {
	return zIndex * numberOfYValues * numberOfXValues +
	       yIndex * numberOfXValues + xIndex;
}

void Picard3D::readDensity3D() {
	auto fluxToDensity = 4_pi * 1_sr / c_light;

	// Read A & Z from file
	int numberOfNucleonsA, chargeNumberZ;
	int attributeIndex = h5File->findAttributeIndex("A of particle");
	h5File->readAttributeFromDataGroup(attributeIndex, numberOfNucleonsA);
	attributeIndex = h5File->findAttributeIndex("Z of particle");
	h5File->readAttributeFromDataGroup(attributeIndex, chargeNumberZ);

	std::vector<int> datasetDimensions(3);
	std::vector<float> datasetContent;

	if (isPIDEnabled(PID(chargeNumberZ, numberOfNucleonsA))) {
		std::cout << "hermes: info: reading file " << h5Filename << std::endl;
		for (std::size_t energyIndex = 0; energyIndex < numberOfEnergies;
		     ++energyIndex) {
			std::string datasetName(getDatasetName(energyIndex));
			h5File->readDataset(datasetName, datasetDimensions, datasetContent);
			for (std::size_t xIndex = 0; xIndex < numberOfXValues; ++xIndex) {
				for (std::size_t yIndex = 0; yIndex < numberOfYValues;
				     ++yIndex) {
					for (std::size_t zIndex = 0; zIndex < numberOfZValues;
					     ++zIndex) {
						// cosmic ray flux x (kinetic energy per nucleon)²
						auto fluxTimesEnergySquared =
						    datasetContent[getArrayIndex3D(xIndex, yIndex,
						                                   zIndex)] *
						    1_MeV / (1_sr * 1_cm2 * 1_s);

						auto kineticEnergyPerNucleon = energyRange[energyIndex];

						auto flux =
						    fluxTimesEnergySquared /
						    (kineticEnergyPerNucleon * kineticEnergyPerNucleon);

						auto density = fluxToDensity * flux;
						grid[energyIndex]->addValue(xIndex, yIndex, zIndex,
						                            density);
					}
				}
			}
		}
	}
}

std::string Picard3D::getDatasetName(std::size_t energyIndex) {
	std::stringstream datasetNameAttribute;
	datasetNameAttribute << "Name_om";
	datasetNameAttribute << std::setfill('0') << std::setw(2) << energyIndex;
	std::string datasetName;
	h5File->readAttributeFromDataGroup(datasetNameAttribute.str(), datasetName);
	return datasetName;
}

QPDensityPerEnergy Picard3D::getDensityPerEnergy(
    const QEnergy &energy, const Vector3QLength &position) const {
	return getDensityPerEnergy(static_cast<int>(energyToIndex.at(energy)),
	                           position);
}

QPDensityPerEnergy Picard3D::getDensityPerEnergy(
    int energyIndex, const Vector3QLength &position) const {
	if (position.x < xMin || position.x > xMax) return {0};
	if (position.y < yMin || position.y > yMax) return {0};
	if (position.z < zMin || position.z > zMax) return {0};

	return (grid[energyIndex])->interpolate(position);
}

std::array<QLength, 2> Picard3D::getXBoundaries() const { return {xMin, xMax}; }

std::array<QLength, 2> Picard3D::getYBoundaries() const { return {yMin, yMax}; }

std::array<QLength, 2> Picard3D::getZBoundaries() const { return {zMin, zMax}; }

}}  // namespace hermes::cosmicrays

#endif  // HERMES_HAVE_HDF5

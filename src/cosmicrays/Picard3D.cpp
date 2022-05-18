#ifdef HERMES_HAVE_HDF5

#include "hermes/cosmicrays/Picard3D.h"

#include <array>
#include <cassert>
#include <filesystem>
#include <iomanip>
#include <iostream>
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
	for (const auto &file : files) {
		std::string filePath = file.path();
		auto searchResult = filePath.find("tfinal");
		bool isFinalTimeStep = searchResult != std::string::npos;
		if (isFinalTimeStep) {
			finalTimeStepDirectory = filePath;
			break;
		}
	}
	return finalTimeStepDirectory;
}

void Picard3D::readFile() {
	std::string finalTimeStepDirectory = findFinalTimeStepDirectory();
	auto speciesFiles =
	    std::filesystem::directory_iterator(finalTimeStepDirectory);
	bool gotEnergyAxisAndSpatialGrid{false};
	for (const auto &speciesFile : speciesFiles) {
		bool isH5File = speciesFile.path().extension() == ".h5";
		if (!isH5File) {
			continue;
		}
		h5File = std::make_unique<Hdf5Reader>(speciesFile.path());
		if (!gotEnergyAxisAndSpatialGrid) {
			readEnergyAxis();
			readSpatialGrid3D();
			gotEnergyAxisAndSpatialGrid = true;
		}
		readDensity3D();
	}
}

void Picard3D::readEnergyAxis() {
	// TODO: This is wrong. Hermes needs the kinetic energy and not the total
	//       energy. I guess that it needs the kinetic energy per nucleon?
	QEnergy energy;
	h5File->readAttributeFromDataGroup("Entries", numberOfEnergies);
	for (int energyIndex = 0; energyIndex < numberOfEnergies; ++energyIndex) {
		std::string datasetName(getDatasetName(energyIndex));
		double energyValue;
		h5File->readAttributeFromDatasetOfDataGroup(datasetName, "Etot",
		                                            energyValue);
		energy = 1_MeV * energyValue;
		energyRange.push_back(energy);
		energyToIndex[energy] = energyIndex;
	}
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

	QLength xGridSize = (xMax - xMin) / (numberOfXValues - 1);
	QLength yGridSize = (yMax - yMin) / (numberOfYValues - 1);
	QLength zGridSize = (zMax - zMin) / (numberOfZValues - 1);

	Vector3d origin(static_cast<double>(xMin), static_cast<double>(yMin),
	                static_cast<double>(zMin));
	Vector3d spacing(static_cast<double>(xGridSize),
	                 static_cast<double>(yGridSize),
	                 static_cast<double>(zGridSize));

	for (int energyIndex = 0; energyIndex < numberOfEnergies; ++energyIndex) {
		grid.push_back(std::make_unique<ScalarGridQPDensityPerEnergy>(
		    ScalarGridQPDensityPerEnergy(origin, numberOfXValues,
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
	// TODO: It is being assumed that the velocity of the cosmic rays is
	//       approximately the speed of light, but this is not true for low
	//       energy particles.
	constexpr double fluxToDensity =
	    static_cast<double>(4_pi / (c_light * 1_GeV));

	// Read A & Z from file
	int numberOfNucleonsA, chargeNumberZ;
	int attributeIndex = h5File->findAttributeIndex("A of particle");
	h5File->readAttributeFromDataGroup(attributeIndex, numberOfNucleonsA);
	attributeIndex = h5File->findAttributeIndex("Z of particle");
	h5File->readAttributeFromDataGroup(attributeIndex, chargeNumberZ);

	std::vector<int> datasetDimensions(3);
	std::vector<float> datasetContent;

	if (isPIDEnabled(PID(chargeNumberZ, numberOfNucleonsA))) {
		std::cerr << "hermes: info: reading species with Z = " << chargeNumberZ
		          << " A = " << numberOfNucleonsA << std::endl;
		for (std::size_t energyIndex = 0; energyIndex < numberOfEnergies;
		     ++energyIndex) {
			std::string datasetName(getDatasetName(energyIndex));
			h5File->readDataset(datasetName, datasetDimensions, datasetContent);
			for (std::size_t xIndex = 0; xIndex < numberOfXValues; ++xIndex) {
				for (std::size_t yIndex = 0; yIndex < numberOfYValues;
				     ++yIndex) {
					for (std::size_t zIndex = 0; zIndex < numberOfZValues;
					     ++zIndex) {
						// cosmicRayFlux in MeV / (sr cm² s)
						double cosmicRayFlux = datasetContent[getArrayIndex3D(
						    xIndex, yIndex, zIndex)];

						// conversion from MeV / (sr cm² s) to GeV / (sr m² s)
						cosmicRayFlux *= 10.;

						double density = fluxToDensity * cosmicRayFlux;
						grid[energyIndex]->addValue(
						    xIndex, yIndex, zIndex,
						    static_cast<QPDensityPerEnergy>(density));
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

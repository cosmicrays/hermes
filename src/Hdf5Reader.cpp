#ifdef HERMES_HAVE_HDF5
#include "hermes/Hdf5Reader.h"

#include <iostream>

Hdf5Reader::Hdf5Reader(std::string filename)
    : filename(std::move(filename)), dataGroup("/Data") {
	openFile();
}

void Hdf5Reader::openFile() {
	hdf5File = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
	if (hdf5File == H5I_INVALID_HID) {
		std::cerr << "hermes: error: Couldn't open the HDF5 file '" << filename
		          << "'." << std::endl;
		std::exit(1);
	}
	h5Group = H5Gopen2(hdf5File, dataGroup.c_str(), H5P_DEFAULT);
	if (h5Group == H5I_INVALID_HID) {
		std::cerr << "hermes: error: Couldn't open the group '" << dataGroup
		          << "' of the HDF5 file '" << filename << "'." << std::endl;
		std::exit(1);
	}
}

void Hdf5Reader::closeFile() {
	herr_t closeGroupError = H5Gclose(h5Group);
	if (closeGroupError < 0) {
		std::cerr << "hermes: warning: An error occurred while closing the "
		             "group '"
		          << dataGroup << "' of the HDF5 file '" << filename << "'."
		          << std::endl;
	}

	herr_t closeFileError = H5Fclose(hdf5File);
	if (closeFileError < 0) {
		std::cerr << "hermes: warning: An error occurred while closing the "
		             "HDF5 file '"
		          << filename << "'." << std::endl;
	}
}

Hdf5Reader::~Hdf5Reader() { closeFile(); }

int Hdf5Reader::findAttributeIndex(const std::string &partOfTheAttributeName) {
	int numberOfAttributes = H5Aget_num_attrs(h5Group);
	assert(numberOfAttributes >= 0);
	for (int attributeIndex = 0; attributeIndex < numberOfAttributes;
	     ++attributeIndex) {
		hid_t attributeID = openAttribute(h5Group, attributeIndex);
		int maximumNameLength = 256;
		char nameBuffer[maximumNameLength];
		ssize_t nameLength =
		    H5Aget_name(attributeID, maximumNameLength, nameBuffer);
		assert(nameLength >= 0);
		closeAttribute(attributeID, attributeIndex);
		std::string attributeName(nameBuffer);
		std::size_t searchResult = attributeName.find(partOfTheAttributeName);
		bool foundAttribute = searchResult != std::string::npos;
		if (foundAttribute) {
			return attributeIndex;
		}
	}
	return -1;
}

void Hdf5Reader::readDataset(const std::string &datasetName,
                             std::vector<int> &datasetDimensions,
                             std::vector<float> &datasetContent) {
	hid_t datasetID = H5Dopen2(h5Group, datasetName.c_str(), H5P_DEFAULT);
	if (datasetID == H5I_INVALID_HID) {
		std::cerr << "hermes: error: Couldn't open the dataset '" << datasetName
		          << "' of the HDF5 file '" << filename << "'." << std::endl;
		std::exit(1);
	}
	hid_t spaceID = H5Dget_space(datasetID);
	assert(spaceID >= 0);
	hssize_t numberOfEntries = H5Sget_simple_extent_npoints(spaceID);
	assert(numberOfEntries >= 0);
	int numberOfDimensions = H5Sget_simple_extent_ndims(spaceID);
	assert(numberOfDimensions >= 0);
	datasetDimensions.resize(numberOfDimensions);
	hsize_t dimensionsBuffer[numberOfDimensions];
	H5Sget_simple_extent_dims(spaceID, dimensionsBuffer, nullptr);
	for (int dimensionIndex = 0; dimensionIndex < numberOfDimensions;
	     ++dimensionIndex) {
		datasetDimensions[dimensionIndex] = dimensionsBuffer[dimensionIndex];
	}

	float rawData[numberOfEntries];
	herr_t readError = H5Dread(datasetID, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
	                           H5P_DEFAULT, rawData);
	if (readError < 0) {
		std::cerr
		    << "hermes: error: An error occurred while reading the dataset '"
		    << datasetName << "' of the HDF5 file '" << filename << "'."
		    << std::endl;
		std::exit(1);
	}

	herr_t closeError = H5Dclose(datasetID);
	if (closeError < 0) {
		std::cerr
		    << "hermes: warning: An error occurred while closing the dataset '"
		    << datasetName << "' of the HDF5 file '" << filename << "'."
		    << std::endl;
	}

	datasetContent.resize(numberOfEntries);
	for (int entryIndex = 0; entryIndex < numberOfEntries; ++entryIndex) {
		datasetContent[entryIndex] = rawData[entryIndex];
	}
}

void Hdf5Reader::readGlobalAttribute(const std::string &attributeName,
                                     std::string &AttributeData) {
	hid_t attributeID = openAttribute(h5Group, attributeName);
	char dataBuffer[MAXIMUM_STRING_LENGTH];
	herr_t readError =
	    H5Aread(attributeID, getHdf5DataType<std::string>(), &dataBuffer);
	if (readError < 0) {
		std::cerr << "hermes: error: Failed to read the attribute '"
		          << attributeName << "' of the group '" << h5Group
		          << "' and the HDF5 file '" << filename << "'." << std::endl;
		std::exit(1);
	}
	AttributeData = dataBuffer;
	closeAttribute(attributeID, attributeName);
}

hid_t Hdf5Reader::openAttribute(hid_t group, std::string attributeName) {
	hid_t attributeID = H5Aopen(group, attributeName.c_str(), H5P_DEFAULT);
	if (attributeID == H5I_INVALID_HID) {
		std::cerr << "hermes: error: Failed to open the attribute '"
		          << attributeName << "' of the HDF5 file '" << filename << "'."
		          << std::endl;
		std::exit(1);
	}
	return attributeID;
}

hid_t Hdf5Reader::openAttribute(hid_t group, int attributeIndex) {
	hid_t attributeID = H5Aopen_idx(group, attributeIndex);
	if (attributeID == H5I_INVALID_HID) {
		std::cerr
		    << "hermes: error: Failed to open the attribute with the index '"
		    << attributeIndex << "' of the HDF5 file '" << filename << "'."
		    << std::endl;
		std::exit(1);
	}
	return attributeID;
}

void Hdf5Reader::closeAttribute(hid_t attributeID, int attributeIndex) {
	herr_t closeError = H5Aclose(attributeID);
	if (closeError < 0) {
		std::cerr
		    << "hermes: warning: Failed to close the attribute with the index '"
		    << attributeIndex << "' of the HDF5 file '" << filename << "'."
		    << std::endl;
	}
}

void Hdf5Reader::closeAttribute(hid_t attributeID,
                                const std::string &attributeName) {
	herr_t closeError = H5Aclose(attributeID);
	if (closeError < 0) {
		std::cerr << "hermes: warning: Failed to close the attribute '"
		          << attributeName << "' of the HDF5 file '" << filename << "'."
		          << std::endl;
	}
}

#endif

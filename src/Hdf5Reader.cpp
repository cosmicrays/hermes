#ifdef HERMES_HAVE_HDF5
#include "hermes/Hdf5Reader.h"

#include <iostream>

Hdf5Reader::Hdf5Reader(const std::string &filename) { openFile(filename); }

hid_t Hdf5Reader::openFile(const std::string &filename) {
	hdf5File = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
	h5Group = H5Gopen2(hdf5File, "/Data", H5P_DEFAULT);
	return 0;
}

hid_t Hdf5Reader::closeFile() { return H5Fclose(hdf5File); }

int Hdf5Reader::findAttributeIndex(
    const std::string &partOfTheAttributeName) {
	int numberOfAttributes = H5Aget_num_attrs(h5Group);
	for (int attributeIndex = 0; attributeIndex < numberOfAttributes;
	     ++attributeIndex) {
		hid_t attributeID = H5Aopen_idx(h5Group, attributeIndex);
		int maximumNameLength = 256;
		char nameBuffer[maximumNameLength];
		H5Aget_name(attributeID, maximumNameLength, nameBuffer);
		H5Aclose(attributeID);
		std::string attributeName(nameBuffer);
		std::size_t searchResult = attributeName.find(partOfTheAttributeName);
		bool foundAttribute = searchResult != std::string::npos;
		if (foundAttribute) {
			return attributeIndex;
		}
	}
	return -1;
}

herr_t Hdf5Reader::readDataset(const std::string &datasetName,
                               std::vector<int> &datasetDimensions,
                               std::vector<float> &datasetContent) {
	hid_t datasetID = H5Dopen2(h5Group, datasetName.c_str(), H5P_DEFAULT);
	hid_t spaceID = H5Dget_space(datasetID);
	hssize_t numberOfEntries = H5Sget_simple_extent_npoints(spaceID);

	int numberOfDimensions = H5Sget_simple_extent_ndims(spaceID);
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

	H5Dclose(datasetID);

	datasetContent.resize(numberOfEntries);
	for (int entryIndex = 0; entryIndex < numberOfEntries; ++entryIndex) {
		datasetContent[entryIndex] = rawData[entryIndex];
	}
	return readError;
}

herr_t Hdf5Reader::readGlobalAttribute(const std::string &attributeName,
                                       std::string &AttributeData) {
	hid_t attributeID = H5Aopen(h5Group, attributeName.c_str(), H5P_DEFAULT);
	char dataBuffer[MAXIMUM_STRING_LENGTH];
	herr_t readError =
	    H5Aread(attributeID, getHdf5DataType<std::string>(), &dataBuffer);
	AttributeData = dataBuffer;
	H5Aclose(attributeID);
	return readError;
}

#endif

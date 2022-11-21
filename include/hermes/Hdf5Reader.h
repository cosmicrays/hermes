/**
 * Hdf5Reader is a class for reading HDF5 files. It has been implemented for the
 * class Picard3D. Picard3D uses this class to import the simulation results
 * from the Software Picard, i.e., cosmic ray fluxes.
 */

#ifdef HERMES_HAVE_HDF5
#ifndef HERMES_HDF5READER_H
#define HERMES_HDF5READER_H

#define MAXIMUM_STRING_LENGTH 256

#include <hdf5.h>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
static hid_t getHdf5DataType();

template <>
hid_t getHdf5DataType<bool>() {
	return H5Tcopy(H5T_NATIVE_INT);
}

template <>
hid_t getHdf5DataType<char>() {
	return H5Tcopy(H5T_NATIVE_CHAR);
}
template <>
hid_t getHdf5DataType<unsigned char>() {
	return H5Tcopy(H5T_NATIVE_UCHAR);
}
template <>
hid_t getHdf5DataType<std::string>() {
	hid_t datatype = H5Tcopy(H5T_C_S1);
	H5Tset_size(datatype, MAXIMUM_STRING_LENGTH);
	return datatype;
}
template <>
hid_t getHdf5DataType<short>() {
	return H5Tcopy(H5T_NATIVE_SHORT);
}
template <>
hid_t getHdf5DataType<unsigned short>() {
	return H5Tcopy(H5T_NATIVE_USHORT);
}
template <>
hid_t getHdf5DataType<int>() {
	return H5Tcopy(H5T_NATIVE_INT);
}
template <>
hid_t getHdf5DataType<unsigned int>() {
	return H5Tcopy(H5T_NATIVE_UINT);
}
template <>
hid_t getHdf5DataType<long>() {
	return H5Tcopy(H5T_NATIVE_LONG);
}
template <>
hid_t getHdf5DataType<unsigned long>() {
	return H5Tcopy(H5T_NATIVE_ULONG);
}
template <>
hid_t getHdf5DataType<long long>() {
	return H5Tcopy(H5T_NATIVE_LLONG);
}
template <>
hid_t getHdf5DataType<unsigned long long>() {
	return H5Tcopy(H5T_NATIVE_ULLONG);
}

template <>
hid_t getHdf5DataType<float>() {
	return H5Tcopy(H5T_NATIVE_FLOAT);
}
template <>
hid_t getHdf5DataType<double>() {
	return H5Tcopy(H5T_NATIVE_DOUBLE);
}
template <>
hid_t getHdf5DataType<long double>() {
	return H5Tcopy(H5T_NATIVE_LDOUBLE);
}

class Hdf5Reader {
  public:
	explicit Hdf5Reader(std::string filename);

	~Hdf5Reader();

	/**
	 * Reads the attribute of the group "/Data" with the index attributeIndex
	 * and saves its data into attributeData.
	 * @tparam T type of the attribute
	 * @param attributeIndex index of the attribute
	 * @param attributeData data of the attribute
	 */
	template <typename T>
	void readAttributeFromDataGroup(int attributeIndex, T &attributeData);

	/**
	 * Reads the attribute of the group "/Data" with the name attributeName and
	 * saves its data into attributeData.
	 * @tparam T type of the attribute
	 * @param attributeName name of the attribute
	 * @param attributeData data of the attribute
	 */
	template <typename T>
	void readAttributeFromDataGroup(const std::string &attributeName,
	                                T &attributeData);

	/**
	 * Reads the string attribute of the group "/Data" with the name
	 * attributeName and saves its data into attributeData.
	 * @param attributeName name of the attribute
	 * @param attributeData data of the attribute
	 */
	void readAttributeFromDataGroup(const std::string &attributeName,
	                                std::string &attributeData);

	/**
	 * Reads the attribute of the group "/Data" with the name attributeName
	 * and saves its data into the vector attributeData.
	 * @tparam T type of the attribute
	 * @param attributeName name of the attribute
	 * @param attributeData data of the attribute
	 */
	template <typename T>
	void readAttributeFromDataGroup(const std::string &attributeName,
	                                std::vector<T> &attributeData);

	/**
	 * Reads the attribute with the name attributeName of the dataset with the
	 * name datasetName of the group "/Data" and saves its data into
	 * attributeData.
	 * @tparam T
	 * @param datasetName name of the dataset of the group "/Data"
	 * @param attributeName name of the attribute
	 * @param attributeData data of the attribute
	 */
	template <typename T>
	void readAttributeFromDatasetOfDataGroup(const std::string &datasetName,
	                                         const std::string &attributeName,
	                                         T &attributeData);

	/**
	 * Finds the index of an attribute via a part of its name.
	 * @param partOfTheAttributeName a part of the attribute name
	 * @return index of the attribute
	 */
	int findAttributeIndex(const std::string &partOfTheAttributeName);

	/**
	 * Reads the dat with the dimensions datasetDimensions of a dataset with
	 * the name datasetName of the group with the name "/Data" and saves its
	 * data into datasetData.
	 * @param datasetName name of the dataset
	 * @param datasetDimensions x, y, z dimensions of the dataset
	 * @param datasetData data of the dataset
	 */
	void readDataset(const std::string &datasetName,
	                 std::vector<int> &datasetDimensions,
	                 std::vector<float> &datasetData);

  private:
	void openFile();
	void closeFile();
	hid_t openAttribute(hid_t objectID, std::string attributeName);
	hid_t openAttribute(hid_t objectID, int attributeIndex);
	void closeAttribute(hid_t attributeID, int attributeIndex);
	void closeAttribute(hid_t attributeID, const std::string &attributeName);
	std::string filename;
	std::string dataGroupName;
	hid_t fileID{};
	hid_t dataGroupID{};
};

template <typename T>
void Hdf5Reader::readAttributeFromDataGroup(int attributeIndex,
                                            T &attributeData) {
	hid_t attributeID = openAttribute(dataGroupID, attributeIndex);
	hid_t readError =
	    H5Aread(attributeID, getHdf5DataType<T>(), &attributeData);
	if (readError < 0) {
		std::cerr
		    << "hermes: error: Failed to read the attribute with the index '"
		    << attributeIndex << "' of the HDF5 file '" << filename << "'."
		    << std::endl;
		std::exit(1);
	}
	closeAttribute(attributeID, attributeIndex);
}

template <typename T>
void Hdf5Reader::readAttributeFromDataGroup(const std::string &attributeName,
                                            T &attributeData) {
	hid_t attributeID = openAttribute(dataGroupID, attributeName);
	herr_t readError =
	    H5Aread(attributeID, getHdf5DataType<T>(), &attributeData);
	if (readError < 0) {
		std::cerr << "hermes: error: Failed to read the attribute '"
		          << attributeName << "' of the group '" << dataGroupID
		          << "' and the HDF5 file '" << filename << "'." << std::endl;
		std::exit(1);
	}
	closeAttribute(attributeID, attributeName);
}

template <typename T>
void Hdf5Reader::readAttributeFromDataGroup(const std::string &attributeName,
                                            std::vector<T> &attributeData) {
	hid_t attributeID = openAttribute(dataGroupID, attributeName);
	hid_t attributeSpace = H5Aget_space(attributeID);
	assert(attributeSpace >= 0);
	hssize_t numberOfEntries = H5Sget_simple_extent_npoints(attributeSpace);
	assert(numberOfEntries >= 0);
	T dataBuffer[numberOfEntries];
	herr_t readError = H5Aread(attributeID, getHdf5DataType<T>(), &dataBuffer);
	if (readError < 0) {
		std::cerr << "hermes: error: Failed to read the attribute '"
		          << attributeName << "' of the group '" << dataGroupID
		          << "' and the HDF5 file '" << filename << "'." << std::endl;
		std::exit(1);
	}
	closeAttribute(attributeID, attributeName);
	attributeData.resize(numberOfEntries);
	for (int entryIndex = 0; entryIndex < numberOfEntries; ++entryIndex) {
		attributeData[entryIndex] = dataBuffer[entryIndex];
	}
}

template <typename T>
void Hdf5Reader::readAttributeFromDatasetOfDataGroup(
    const std::string &datasetName, const std::string &attributeName,
    T &attributeData) {
	hid_t datasetID = H5Dopen2(dataGroupID, datasetName.c_str(), H5P_DEFAULT);
	if (datasetID == H5I_INVALID_HID) {
		std::cerr << "hermes: error: Failed to open the dataset '"
		          << datasetName << "' of the HDF5 file '" << fileID << "'."
		          << std::endl;
		std::exit(1);
	}
	hid_t attributeID = openAttribute(datasetID, attributeName);
	hid_t readError =
	    H5Aread(attributeID, getHdf5DataType<T>(), &attributeData);
	if (readError < 0) {
		std::cerr << "hermes: error: Failed to read the attribute '"
		          << attributeName << "' of the dataset '" << datasetName
		          << "' and the HDF5 file '" << filename << "'." << std::endl;
		std::exit(1);
	}
	closeAttribute(attributeID, attributeName);
	herr_t closeError = H5Dclose(datasetID);
	if (closeError < 0) {
		std::cerr << "hermes: warning: Failed to close the dataset '"
		          << datasetName << "' of the file '" << filename << "'."
		          << std::endl;
	}
}

#endif
#endif

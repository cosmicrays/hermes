#ifdef HERMES_HAVE_HDF5
#ifndef HERMES_HDF5READER_H
#define HERMES_HDF5READER_H

#define MAXIMUM_STRING_LENGTH 256

#include <hdf5.h>

#include <string>
#include <vector>

template <typename T>
using Invoke = typename T::type;
template <typename Condition>
using negation = std::integral_constant<bool, !bool(Condition::value)>;
template <typename Condition>
using EnableIf = Invoke<std::enable_if<Condition::value>>;
template <typename Condition>
using DisableIf = EnableIf<negation<Condition>>;

#define template_noPointer \
	template <typename T, typename = DisableIf<std::is_pointer<T>>>

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
	explicit Hdf5Reader(const std::string &filename);
	hid_t openFile(const std::string &filename);
	/*!Read an attribute from the group '/Data' via the index*/
	template_noPointer hid_t readGlobalAttribute(int attributeIndex,
	                                             T &attributeData);
	/*!Read an attribute from Data group*/
	template_noPointer hid_t
	readGlobalAttribute(const std::string &attributeName, T &attributeData);
	/*!Read a string attribute from Data group*/
	herr_t readGlobalAttribute(const std::string &attributeName,
	                           std::string &AttributeData);
	/*!Read an array of attribute from Data group*/
	template_noPointer hid_t readGlobalAttribute(
	    const std::string &attributeName, std::vector<T> &attributeData);
	/*!Read an attribute related to a dataset */
	template_noPointer hid_t readAttributeFromDataset(
	    const std::string &datasetName, const std::string &attributeName,
	    T &attributeData);
	/*!Find index of argument by using part of the name*/
	int findAttributeIndex(const std::string &partOfTheAttributeName);

	herr_t readDataset(const std::string &datasetName,
	                   std::vector<int> &datasetDimensions,
	                   std::vector<float> &datasetContent);

  private:
	hid_t closeFile() const;
	hid_t hdf5File{}, h5Group{};
};

template <typename T, typename>
hid_t Hdf5Reader::readGlobalAttribute(int attributeIndex, T &attributeData) {
	hid_t attributeID = H5Aopen_idx(h5Group, attributeIndex);
	hid_t readError =
	    H5Aread(attributeID, getHdf5DataType<T>(), &attributeData);
	H5Aclose(attributeID);
	return readError;
}

template <typename T, typename>
hid_t Hdf5Reader::readGlobalAttribute(const std::string &attributeName,
                                      T &attributeData) {
	hid_t attributeID = H5Aopen(h5Group, attributeName.c_str(), H5P_DEFAULT);
	hid_t readError =
	    H5Aread(attributeID, getHdf5DataType<T>(), &attributeData);
	H5Aclose(attributeID);
	return readError;
}

template <typename T, typename>
hid_t Hdf5Reader::readGlobalAttribute(const std::string &attributeName,
                                      std::vector<T> &attributeData) {
	hid_t attributeID = H5Aopen(h5Group, attributeName.c_str(), H5P_DEFAULT);
	hid_t attributeSpace = H5Aget_space(attributeID);
	hssize_t numberOfEntries = H5Sget_simple_extent_npoints(attributeSpace);
	T dataBuffer[numberOfEntries];
	hid_t readError = H5Aread(attributeID, getHdf5DataType<T>(), &dataBuffer);
	H5Aclose(attributeID);
	attributeData.resize(numberOfEntries);
	for (int entryIndex = 0; entryIndex < numberOfEntries; ++entryIndex) {
		attributeData[entryIndex] = dataBuffer[entryIndex];
	}
	return readError;
}

template <typename T, typename>
hid_t Hdf5Reader::readAttributeFromDataset(const std::string &datasetName,
                                           const std::string &attributeName,
                                           T &attributeData) {
	hid_t datasetID = H5Dopen2(h5Group, datasetName.c_str(), H5P_DEFAULT);
	hid_t attributeID = H5Aopen(datasetID, attributeName.c_str(), H5P_DEFAULT);
	hid_t readError =
	    H5Aread(attributeID, getHdf5DataType<T>(), &attributeData);
	H5Aclose(attributeID);
	H5Dclose(datasetID);
	return readError;
}

#endif
#endif

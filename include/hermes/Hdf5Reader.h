#ifdef HERMES_HAVE_HDF5
#ifndef HERMES_HDF5READER_H
#define HERMES_HDF5READER_H

#define MAXIMUM_STRING_LENGTH 256

#include <hdf5.h>

#include <string>
#include <vector>

// Some definitions for convenience
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
static hid_t get_hdf5_data_type();

template <>
hid_t get_hdf5_data_type<bool>() {
	return H5Tcopy(H5T_NATIVE_INT);
}

template <>
hid_t get_hdf5_data_type<char>() {
	return H5Tcopy(H5T_NATIVE_CHAR);
}
template <>
hid_t get_hdf5_data_type<unsigned char>() {
	return H5Tcopy(H5T_NATIVE_UCHAR);
}
template <>
hid_t get_hdf5_data_type<std::string>() {
	hid_t datatype = H5Tcopy(H5T_C_S1);
	H5Tset_size(datatype, MAXIMUM_STRING_LENGTH);
	return datatype;
}
template <>
hid_t get_hdf5_data_type<short>() {
	return H5Tcopy(H5T_NATIVE_SHORT);
}
template <>
hid_t get_hdf5_data_type<unsigned short>() {
	return H5Tcopy(H5T_NATIVE_USHORT);
}
template <>
hid_t get_hdf5_data_type<int>() {
	return H5Tcopy(H5T_NATIVE_INT);
}
template <>
hid_t get_hdf5_data_type<unsigned int>() {
	return H5Tcopy(H5T_NATIVE_UINT);
}
template <>
hid_t get_hdf5_data_type<long>() {
	return H5Tcopy(H5T_NATIVE_LONG);
}
template <>
hid_t get_hdf5_data_type<unsigned long>() {
	return H5Tcopy(H5T_NATIVE_ULONG);
}
template <>
hid_t get_hdf5_data_type<long long>() {
	return H5Tcopy(H5T_NATIVE_LLONG);
}
template <>
hid_t get_hdf5_data_type<unsigned long long>() {
	return H5Tcopy(H5T_NATIVE_ULLONG);
}

template <>
hid_t get_hdf5_data_type<float>() {
	return H5Tcopy(H5T_NATIVE_FLOAT);
}
template <>
hid_t get_hdf5_data_type<double>() {
	return H5Tcopy(H5T_NATIVE_DOUBLE);
}
template <>
hid_t get_hdf5_data_type<long double>() {
	return H5Tcopy(H5T_NATIVE_LDOUBLE);
}

class Hdf5Reader {
  public:
	Hdf5Reader(std::string filename, int _suppress_verbosity = 0);
	hid_t open_file(std::string filename, int _suppress_verbosity = 0);
	/*!Read an attribute from Data group by Index*/
	template_noPointer hid_t ReadGlobalAttribute(int indexAttr, T &AttrData);
	/*!Read an attribute from Data group*/
	template_noPointer hid_t ReadGlobalAttribute(std::string AttrName,
	                                             T &AttrData);
	/*!Read a string attribute from Data group*/
	herr_t ReadGlobalAttribute(std::string AttrName, std::string &AttrData);
	/*!Read an array of attribute from Data group*/
	template_noPointer hid_t ReadGlobalAttribute(std::string AttrName,
	                                             std::vector<T> &AttrData);
	/*!Read an attribute related to a dataset */
	template_noPointer hid_t ReadAttributeFromDataset(
	    std::string DataSetName, const std::string &AttrName, T &AttrData);
	/*!Find index of argument by using part of the name*/
	int FindAttrIndex(std::string AttrNamePart);

	herr_t ReadDataset(std::string dsetName, std::vector<int> &dims,
	                   std::vector<float> &data);

  private:
	hid_t close_file();
	hid_t hdf5file, h5group;
	int suppress_verbosity;
};

template <typename T, typename>
hid_t Hdf5Reader::ReadGlobalAttribute(int indexAttr, T &AttrData) {
	// Open Attribute using given index:
	hid_t idAttr = H5Aopen_idx(h5group, indexAttr);
	// Read Attribute Data
	hid_t return_val = H5Aread(idAttr, get_hdf5_data_type<T>(), &AttrData);
	H5Aclose(idAttr);
	return return_val;
}

template <typename T, typename>
hid_t Hdf5Reader::ReadGlobalAttribute(std::string AttrName, T &AttrData) {
	// Open Attribute
	hid_t idAttr = H5Aopen(h5group, AttrName.c_str(), H5P_DEFAULT);
	// Read Attribute Data
	hid_t return_val = H5Aread(idAttr, get_hdf5_data_type<T>(), &AttrData);
	H5Aclose(idAttr);
	return return_val;
}

template <typename T, typename>
hid_t Hdf5Reader::ReadGlobalAttribute(std::string AttrName,
                                      std::vector<T> &AttrData) {
	// Open Attribute
	hid_t attr = H5Aopen(h5group, AttrName.c_str(), H5P_DEFAULT);

	// Get type of attribute
	hid_t atype = H5Aget_type(attr);
	// Get attribute space
	hid_t aspace = H5Aget_space(attr);
	// Get rank of attribute
	int nEntries = H5Sget_simple_extent_npoints(aspace);

	T rawData[nEntries];

	// Read Attribute Data
	hid_t return_val = H5Aread(attr, get_hdf5_data_type<T>(), &rawData);
	H5Aclose(attr);

	AttrData.resize(nEntries);

	for (int iEntry = 0; iEntry < nEntries; ++iEntry) {
		AttrData[iEntry] = rawData[iEntry];
	}

	return return_val;
}

template <typename T, typename>
hid_t Hdf5Reader::ReadAttributeFromDataset(std::string DataSetName,
                                           const std::string &AttrName,
                                           T &AttrData) {
	// Open the dataset
	hid_t dataset = H5Dopen2(h5group, DataSetName.c_str(), H5P_DEFAULT);

	hid_t attr = H5Aopen(dataset, AttrName.c_str(), H5P_DEFAULT);

	hid_t err = H5Aread(attr, get_hdf5_data_type<T>(), &AttrData);

	H5Aclose(attr);
	H5Dclose(dataset);

	return err;
}

#endif
#endif

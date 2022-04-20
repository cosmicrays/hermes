#ifdef HERMES_HAVE_HDF5
#ifndef HERMES_HDF5READER_H
#define HERMES_HDF5READER_H

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
};

template <>
hid_t get_hdf5_data_type<char>() {
	return H5Tcopy(H5T_NATIVE_CHAR);
};
template <>
hid_t get_hdf5_data_type<unsigned char>() {
	return H5Tcopy(H5T_NATIVE_UCHAR);
};
template <>
hid_t get_hdf5_data_type<std::string>() {
	hid_t datatype = H5Tcopy(H5T_C_S1);
	// Set size to be variable
	H5Tset_size(datatype, H5T_VARIABLE);
	return datatype;
};
template <>
hid_t get_hdf5_data_type<short>() {
	return H5Tcopy(H5T_NATIVE_SHORT);
};
template <>
hid_t get_hdf5_data_type<unsigned short>() {
	return H5Tcopy(H5T_NATIVE_USHORT);
};
template <>
hid_t get_hdf5_data_type<int>() {
	return H5Tcopy(H5T_NATIVE_INT);
};
template <>
hid_t get_hdf5_data_type<unsigned int>() {
	return H5Tcopy(H5T_NATIVE_UINT);
};
template <>
hid_t get_hdf5_data_type<long>() {
	return H5Tcopy(H5T_NATIVE_LONG);
};
template <>
hid_t get_hdf5_data_type<unsigned long>() {
	return H5Tcopy(H5T_NATIVE_ULONG);
};
template <>
hid_t get_hdf5_data_type<long long>() {
	return H5Tcopy(H5T_NATIVE_LLONG);
};
template <>
hid_t get_hdf5_data_type<unsigned long long>() {
	return H5Tcopy(H5T_NATIVE_ULLONG);
};

template <>
hid_t get_hdf5_data_type<float>() {
	return H5Tcopy(H5T_NATIVE_FLOAT);
};
template <>
hid_t get_hdf5_data_type<double>() {
	return H5Tcopy(H5T_NATIVE_DOUBLE);
};
template <>
hid_t get_hdf5_data_type<long double>() {
	return H5Tcopy(H5T_NATIVE_LDOUBLE);
};

class Hdf5Reader {
  public:
	Hdf5Reader(std::string filename, int _suppress_verbosity = 0);
	hid_t open_file(std::string filename, int _suppress_verbosity = 0);
	/*!Read an attribute from Data group by Index*/
	template_noPointer hid_t ReadGlobalAttribute(int indexAttr, T &AttrData);
	/*!Read an attribute from Data group*/
	template_noPointer hid_t ReadGlobalAttribute(std::string AttrName,
	                                             T &AttrData);
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

#endif
#endif

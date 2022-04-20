#ifdef HERMES_HAVE_HDF5
#include "hermes/Hdf5Reader.h"
#include <iostream>

Hdf5Reader::Hdf5Reader(std::string filename, int _suppress_verbosity)
{
	open_file(filename, _suppress_verbosity);
}

hid_t Hdf5Reader::open_file(std::string filename, int _suppress_verbosity)
{
	this->suppress_verbosity = _suppress_verbosity;

	hdf5file =  H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
	h5group = H5Gopen2(hdf5file, "/Data", H5P_DEFAULT);

	return 0;

}

hid_t Hdf5Reader::close_file() {
	return H5Fclose(hdf5file);
}

int Hdf5Reader::FindAttrIndex(std::string AttrNamePart) {
	// get number of attributes in group
	int numAttrs = H5Aget_num_attrs(h5group);

	// Loop over all attributes
	for(unsigned int iAttr=0; iAttr<numAttrs; ++iAttr) {
		// Open attribute
		hid_t idAttr = H5Aopen_idx(h5group, iAttr);
		// get name of attribute
		int maxName = 256;
		char bufName[maxName];
		H5Aget_name(idAttr, maxName, bufName);

		H5Aclose(idAttr);

		std::string attrName(bufName);

		std::size_t found = attrName.find(AttrNamePart);
		// part was found at
		if(found!=std::string::npos) {
			return iAttr;
		}
	}
	return -1;
}

template <typename T, typename>
hid_t Hdf5Reader::ReadGlobalAttribute(int indexAttr, T &AttrData) {
	// Open Attribute using given index:
	hid_t idAttr = H5Aopen_idx(h5group, indexAttr );
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
hid_t Hdf5Reader::ReadGlobalAttribute(std::string AttrName, std::vector<T> &AttrData) {
  // Open Attribute
  hid_t attr = H5Aopen(h5group, AttrName.c_str(), H5P_DEFAULT);

  // Get type of attribute
  hid_t atype  = H5Aget_type(attr);
  // Get attribute space
  hid_t aspace = H5Aget_space(attr);
  // Get rank of attribute
  int nEntries = H5Sget_simple_extent_npoints(aspace);

  T rawData[nEntries];

  // Read Attribute Data
  hid_t return_val = H5Aread(attr, get_hdf5_data_type<T>(), &rawData);
  H5Aclose(attr);

  AttrData.resize(nEntries);

  for(int iEntry=0; iEntry<nEntries; ++iEntry) {
	  AttrData[iEntry] = rawData[iEntry];
  }

  return return_val;
}


template <typename T, typename>
hid_t Hdf5Reader::ReadAttributeFromDataset(std::string DataSetName,
		const std::string &AttrName, T &AttrData) {
  // Open the dataset
  hid_t dataset = H5Dopen2(h5group, DataSetName.c_str(), H5P_DEFAULT);

  hid_t attr = H5Aopen(dataset, AttrName.c_str(), H5P_DEFAULT);



  hid_t err = H5Aread(attr, get_hdf5_data_type<T>(), &AttrData);

  H5Aclose(attr);
  H5Dclose(dataset);

  return err;
}

herr_t Hdf5Reader::ReadDataset(std::string DataSetName, std::vector<int> &dims, std::vector<float> &data) {
	// Open the dataset
	hid_t idDataset = H5Dopen2(h5group, DataSetName.c_str(), H5P_DEFAULT);

	// Get dataset space
	hid_t idSpace = H5Dget_space(idDataset);

	// Get number of data points
	int nEntries = H5Sget_simple_extent_npoints(idSpace);

	// Determine dimensionality and store in dims variable
	int dimhdf = H5Sget_simple_extent_ndims(idSpace);
	dims.resize(dimhdf);
	hsize_t dimsInput[dimhdf];
	H5Sget_simple_extent_dims(idSpace, dimsInput, NULL);
	for(int iDir=0; iDir<dimhdf; ++iDir) {
		dims[iDir] = dimsInput[iDir];
	}

	float rawData[nEntries];
	herr_t readErr = H5Dread(idDataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, rawData);

	// Close the dataset
	H5Dclose(idDataset);

	// Store raw data in std vector
	data.resize(nEntries);

	for(int iEntry=0; iEntry<nEntries; ++iEntry) {
		data[iEntry] = rawData[iEntry];
	}
	return readErr;
}


#endif

#ifdef HERMES_HAVE_CFITSIO

#include "hermes/FITSWrapper.h"

#include <iostream>
#include <stdexcept>

namespace hermes {

FITSFile::FITSFile(const std::string &filename_) : filename(filename_) { };

FITSFile::~FITSFile() {
	fits_close_file(fptr, &status);
}

void FITSFile::createFile() {
	if (fits_create_file(&fptr, filename.c_str(), &status))
		fits_report_error(stderr, status);
	if (status != 0)
		throw std::runtime_error("Cannot create file."); 
}

void FITSFile::deleteFile() {
	if (fits_delete_file(fptr, &status))
	        fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot delete file.");
}

void FITSFile::openFile(IOMode ioMode) {
	if (fits_open_file(&fptr, filename.c_str(), static_cast<int>(ioMode), &status))
                fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot open file.");
}

void FITSFile::moveToHDU(int hduNumber) {
	if (fits_movabs_hdu(fptr, hduNumber, &hduType, &status)) 
                fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot move to the specified HDU.");
}

int FITSFile::getNumberOfHDUs() {
	int hduNumber;
	if (fits_get_num_hdus(fptr, &hduNumber, &status))
	      fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Could not retrieve the HDU number.");
	return hduNumber;
}

int FITSFile::getCurrentHDUNumber() {
	int hduActual;
	if (fits_get_hdu_num(fptr, &hduActual))
		fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Could not retrieve the selected HDU number.");
        return hduActual;
}

void FITSFile::createImage(int bitpix, int naxis, long *naxes) {
	if (fits_create_img(fptr, bitpix, naxis, naxes, &status))
		fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot create image in FITS file.");
}

void FITSFile::writeImage(int dataType, int firstElement,
		int nElements, void *array) {	
	if (fits_write_img(fptr, dataType, firstElement, nElements, array, &status))
		fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot write image in FITS file.");
}

// TODO: ugly - should be rewritten
std::unique_ptr<std::vector<float> > FITSFile::readImageAsFloat(int firstElement, int nElements) {
	std::unique_ptr<std::vector<float> > resultArray =
		std::make_unique<std::vector<float> >(nElements);

	void * arrayPtr = static_cast<void*>(resultArray->data());
	float * nullval = nullptr;
	int dataType = TFLOAT;
	int anynul = -1;

	if (fits_read_img(fptr, dataType, firstElement, nElements, nullval,
				arrayPtr, &anynul, &status)) 
		fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot read image.");

	return resultArray; 
}

void FITSFile::createTable(int tableType, long int nRows, int nColumns, char *columnName[],
		char *columnType[], char *columnUnit[], const char *tableName) {
	if (fits_create_tbl(fptr, tableType, nRows, nColumns,
	    columnName, columnType, columnUnit, tableName, &status))
		fits_report_error(stderr, status);
}
	
void FITSFile::writeDate() {
	if (fits_write_date(fptr, &status))
		fits_report_error(stderr, status);
	if (status != 0)
		throw std::runtime_error("Cannot write date."); 
}

void FITSFile::writeKeyValue(FITSKeyValue &kv, const char comment[]) {
	if (fits_write_key(fptr, kv.getType(), kv.getKey(), kv.getValue(), comment, &status))
		fits_report_error(stderr, status);
	if (status != 0)
		throw std::runtime_error("Cannot write key-value pair."); 
}

template<typename T>
int fits_read_key_by_type(fitsfile *fptr, int datatype, char *keyname, T *value,
       char *comment, int *status) {
	return fits_read_key(fptr, datatype, keyname, value,  comment, status);
}

//template int fits_read_key_by_type<int>(fitsfile *fptr, int datatype, char *keyname, int *value,
//       char *comment, int *status);

FITSKeyValue FITSFile::readKeyValue(std::string key, int type) {
	auto kv = FITSKeyValue(key);
	kv.setType(type);

	//if (fits_read_key(fptr, kv.getType(), kv.getKey(), &tmpValue,  NULL, &status))
	//     fits_report_error(stderr, status);

	
	switch (type) {
		case TSTRING : fits_read_key(fptr, kv.getType(), kv.getKey(), &kv.s,  NULL, &status); //kv.s = (static_cast<char *>(tmpValue));
		case TINT : fits_read_key(fptr, kv.getType(), kv.getKey(), &kv.i,  NULL, &status); //kv.i = *(static_cast<int *>(tmpValue));
		case TLONG : fits_read_key(fptr, kv.getType(), kv.getKey(), &kv.l,  NULL, &status); //kv.l = *(static_cast<long int *>(tmpValue));
		case TFLOAT : fits_read_key(fptr, kv.getType(), kv.getKey(), &kv.f,  NULL, &status); //kv.f = *(static_cast<float *>(tmpValue));
		case TDOUBLE : fits_read_key(fptr, kv.getType(), kv.getKey(), &kv.d,  NULL, &status); //kv.d = *(static_cast<double *>(tmpValue));
	}

	if (status != 0)
		throw std::runtime_error("Cannot read key-value pair.");
	
	return kv;
}

void FITSFile::writeColumn(int dataType, int column, long int firstRow, long int firstElement,
		long int nElements, void *array) {
	if(fits_write_col(fptr, dataType, column, firstRow, firstElement, nElements,
				array, &status))
		fits_report_error(stderr, status);
}

} // namespace hermes

#endif // HERMES_HAVE_CFITSIO

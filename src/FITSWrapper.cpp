#ifdef HERMES_HAVE_CFITSIO

#include "hermes/FITSWrapper.h"

#include <iostream>
#include <stdexcept>

namespace hermes {

FITSFile::FITSFile(const std::string &filename_) : filename(filename_) {
};

FITSFile::~FITSFile() {
	if (fptr != nullptr)
		closeFile();
}

/* File-related operations */

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
	fptr = nullptr;
}

void FITSFile::openFile(FITS::IOMode ioMode) {
	if (fits_open_file(&fptr, filename.c_str(), ioMode, &status))
                fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot open file.");
}

void FITSFile::closeFile() {
	if (fits_close_file(fptr, &status))
		fits_report_error(stderr, status);
	if (status != 0)
                std::cerr << "CFITSIO error: Status code non-zero when closing file. Potentially corrupted file." << std::endl;
	fptr = nullptr;
}

/* HDU-related operations */

void FITSFile::moveToHDU(int hduNumber_) {
	int readHDUType;
	if (fits_movabs_hdu(fptr, hduNumber_, &readHDUType, &status)) 
                fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot move to the specified HDU.");
	hduType = intToHDUType(readHDUType);
}

int FITSFile::getNumberOfHDUs() {
	int hduNumber;
	if (fits_get_num_hdus(fptr, &hduNumber, &status))
	      fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot retrieve the HDU number.");
	return hduNumber;
}

int FITSFile::getCurrentHDUNumber() {
	int hduActual;
	if (fits_get_hdu_num(fptr, &hduActual))
		fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot retrieve the selected HDU number.");
        return hduActual;
}

FITS::HDUType FITSFile::intToHDUType(int hduType_) {
	switch (hduType_) {
		case IMAGE_HDU : return FITS::IMAGE;
		case ASCII_TBL : return FITS::ASCII;
		case BINARY_TBL : return FITS::BINARY;
		default : throw std::runtime_error("Unknown HDU type!");
	}
}

FITS::HDUType FITSFile::getHDUType() {
	int readHDUType;
	if (fits_get_hdu_type(fptr,  &readHDUType, &status))
		fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot retrieve the type of the selected HDU.");
	return intToHDUType(hduType);
}

int FITSFile::getNumOfKeywords() {
	int keyexist, morekeys = 0;
	if (fits_get_hdrspace(fptr, &keyexist, &morekeys, &status))
		fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot access the number of keyword in this header.");
	return keyexist;
}


std::vector<std::string> FITSFile::getHeaderRecords() {
	char readRecord[80];
	std::vector<std::string> records;

	for (int i = 1; i <= getNumOfKeywords(); ++i) {
		if (fits_read_record(fptr, i, readRecord, &status))
			fits_report_error(stderr, status);
		records.push_back(static_cast<std::string>(readRecord));
	}
	if (status != 0)
		throw std::runtime_error("Cannot read header records from this header.");

	return records;
}

void FITSFile::writeDate() {
	if (fits_write_date(fptr, &status))
		fits_report_error(stderr, status);
	if (status != 0)
		throw std::runtime_error("Cannot write date."); 
}

void FITSFile::writeKeyValue(FITSKeyValue &kv, const char comment[]) {
	if (fits_write_key(fptr, kv.getType(), kv.getKey(), kv.getValueAsVoid(), comment, &status))
		fits_report_error(stderr, status);
	if (status != 0)
		throw std::runtime_error("Cannot write key-value pair."); 
}

FITSKeyValue FITSFile::readKeyValue(std::string key_, FITS::DataType type_) {
	FITSKeyValue kv = FITSKeyValue(key_);
	kv.setType(type_);

	//TODO: rewrite	
	switch (kv.getType()) {
		case FITS::STRING :
			fits_read_key(fptr, kv.getType(), kv.getKey(), kv.s,  NULL, &status);
		case FITS::INT :
			fits_read_key(fptr, kv.getType(), kv.getKey(), &kv.i,  NULL, &status);
		case FITS::LONG :
			fits_read_key(fptr, kv.getType(), kv.getKey(), &kv.l,  NULL, &status);
		case FITS::FLOAT :
			fits_read_key(fptr, kv.getType(), kv.getKey(), &kv.f,  NULL, &status);
		case FITS::DOUBLE :
			fits_read_key(fptr, kv.getType(), kv.getKey(), &kv.d,  NULL, &status);
	}

	if (status != 0)
		throw std::runtime_error("Cannot read key-value pair.");
	
	return kv;
}

/* Image-related operations */

void FITSFile::createImage(FITS::ImgType bitpix, int naxis, long *naxes) {
	if (fits_create_img(fptr, static_cast<int>(bitpix), naxis, naxes, &status))
		fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot create image in FITS file.");
}

void FITSFile::writeImage(FITS::DataType dataType, int firstElement,
		int nElements, void *array) {	
	if (fits_write_img(fptr, static_cast<int>(dataType), firstElement, nElements, array, &status))
		fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot write image in FITS file.");
}

std::vector<float> FITSFile::readImageAsFloat(unsigned int firstElement, unsigned int nElements) {
	std::vector<float> resultArray(nElements, 0);
	void * arrayPtr = static_cast<void*>(resultArray.data());
	float nullval = 0;
	int dataType = TFLOAT;
	int anynul = -1;
	
	if (nElements == 0)
		throw std::runtime_error("Cannot read image of size 0.");
	if (fits_read_img(fptr, dataType, firstElement, nElements, &nullval,
				arrayPtr, &anynul, &status)) 
		fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot read image.");

	return resultArray; 
}

void FITSFile::createTable(FITS::HDUType tableType, long int nRows, int nColumns, char *columnName[],
		char *columnType[], char *columnUnit[], const char *tableName) {
	if (fits_create_tbl(fptr, tableType, nRows, nColumns,
	    columnName, columnType, columnUnit, tableName, &status))
		fits_report_error(stderr, status);
	if (status != 0)
		throw std::runtime_error("Cannot create table."); 
}
	

void FITSFile::writeColumn(FITS::DataType dataType, int column, long int firstRow, long int firstElement,
		long int nElements, void *array) {
	if(fits_write_col(fptr, static_cast<int>(dataType), column, firstRow, firstElement, nElements,
				array, &status))
		fits_report_error(stderr, status);
        if (status != 0)
                throw std::runtime_error("Cannot write a column.");
}

} // namespace hermes

#endif // HERMES_HAVE_CFITSIO

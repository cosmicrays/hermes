#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_FITSWRAPPER_H
#define HERMES_FITSWRAPPER_H

#include "fitsio.h"
#include <vector>
#include <memory>
#include <string>
#include <cstring>

namespace hermes {

namespace FITS {
	enum DataType {INT = TINT, LONG = TLONG, FLOAT = TFLOAT,
		DOUBLE = TDOUBLE, STRING = TSTRING};
	enum IOMode {READ = READONLY, WRITE = READWRITE};
	enum HDUType {IMAGE = IMAGE_HDU, ASCII = ASCII_TBL,
		BINARY = BINARY_TBL};
}

struct FITSKeyValue {
	std::string key;
	FITS::DataType type;
	//TODO: implement through std::variant, but it requires C++17
	union {
		int i;
		long int l;
		float f;
		double d;
		char s[80];
	};
	void * value_ptr;
public:
	FITS::DataType getType() { return type; }
	void setType(FITS::DataType type_) { type = type_; }
	void setTypeAsInt(int type_) { 
		switch (type) {
			case TINT : type = FITS::INT;
			case TLONG : type = FITS::LONG;
			case TFLOAT : type = FITS::FLOAT;
			case TDOUBLE : type = FITS::DOUBLE;
			case TSTRING : type = FITS::STRING;
		}
	}
	const char * getKey() { return &key[0]; }
	void * getValueAsVoid() { return value_ptr; }
	std::string getValueAsString() {
		return static_cast<std::string>(s);
	}
	int getValueAsInt() { return i; }
	double getValueAsDouble() { return d; }

	FITSKeyValue(std::string key_, std::string value_) : 
		type(FITS::STRING), key(key_) {
			// ugly?
			strncpy(s, value_.c_str(), sizeof(s));
			s[sizeof(s) - 1] = 0;
		        value_ptr = static_cast<void *>(s);
	};
	FITSKeyValue(std::string key_, int value_) :
		type(FITS::INT), key(key_) {
			i = value_;
			value_ptr = &i;
	};
	FITSKeyValue(std::string key_, std::size_t value_) :
		type(FITS::LONG), key(key_) {
			l = value_;
		        value_ptr = &l;
	};
	FITSKeyValue(std::string key_, long int value_) :
		type(FITS::LONG), key(key_) {
			l = value_;	
		        value_ptr = &l;
	};
	FITSKeyValue(std::string key_, float value_) :
		type(FITS::FLOAT), key(key_) {
			f = value_;	
		        value_ptr = &f;
	};
	FITSKeyValue(std::string key_, double value_) :
		type(FITS::DOUBLE), key(key_) {
			d = value_;	
		        value_ptr = &d;
	};
	FITSKeyValue(std::string key_) : key(key_) { };
};


class FITSFile {
private:
	std::string filename;
	fitsfile *fptr = 0;
	int status = 0;
	
	FITS::HDUType hduType = FITS::IMAGE; 
	FITS::HDUType intToHDUType(int hduType_);
	int HDUDataTypeToInt(FITS::DataType dataType_);
public:
	FITSFile(const std::string &filename_);
	~FITSFile();
	
	inline int getStatus() { return status; }

	/* File-related operations */
	void createFile();
	void deleteFile();
	void openFile(FITS::IOMode ioMode = FITS::READ);
	void closeFile();

	/* HDU-related operations */
	void moveToHDU(int hduNumber);
	int getNumberOfHDUs();
	int getCurrentHDUNumber();
	FITS::HDUType getHDUType();
	int getNumOfKeywords();
	std::vector<std::string> getHeaderRecords();
	void writeKeyValue(FITSKeyValue &kv, const char comment[]);
	
	FITSKeyValue readKeyValue(std::string key_, FITS::DataType type_);
	inline std::string readKeyValueAsString(std::string key_) {
		return (readKeyValue(key_, FITS::STRING)).getValueAsString();
	};
	inline double readKeyValueAsDouble(std::string key_) {
		return (readKeyValue(key_, FITS::DOUBLE)).getValueAsDouble();
	};
	inline int readKeyValueAsInt(std::string key_) {
		return (readKeyValue(key_, FITS::INT)).getValueAsInt();
	};

	/* Image-related operations */
	void createImage(int bitpix, int naxis, long *naxes);
	void writeImage(FITS::DataType dtype, int firstElement,
			int nElements, void *array);
	std::unique_ptr<std::vector<float> > readImageAsFloat(int firstElement, int nElements);

	void createTable(int tableType, long int nRows, int nColumns, char *columnName[],
                char *columnType[], char *columnUnit[], const char *tableName);
	void writeDate();
	void writeColumn(FITS::DataType dataType, int column, long int firstRow,
			long int firstElement, long int nElements, void *array);

};

} // namespace hermes

#endif // HERMES_FITSWRAPPER_H

#endif // HERMES_HAVE_CFITSIO

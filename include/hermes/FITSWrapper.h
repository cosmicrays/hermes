#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_FITSWRAPPER_H
#define HERMES_FITSWRAPPER_H

#include <string>
#include "fitsio.h"
#include <vector>
#include <memory>

namespace hermes {

struct FITSKeyValue {
	std::string key;
	//TODO: implement through std::variant, but it requires C++17
	int type;
	//enum FType {INT = TINT, LONG = TLONG, FLOAT = TFLOAT,
	//	DOUBLE = TDOUBLE, STRING = TSTRING} type;
	union {
		int i;
		long int l;
		float f;
		double d;
		char * s;
	};
	void * value_ptr;
public:
	int getType() {	return type; }
	void setType(int type_) { type = type_; }
	const char * getKey() { return &key[0]; }
	void * getValue() { return value_ptr; }
	std::string getValueAsString() {
		return static_cast<std::string>(s);
	}
	int getValueAsInt() {
		return i;
	}
	double getValueAsDouble() {
		return d;
	}

	FITSKeyValue(std::string key_, std::string value_) : 
		type(TSTRING), key(key_) {
		       s = &value_[0];
		       value_ptr = &s;
	};
	FITSKeyValue(std::string key_, int value_) :
		type(TINT), key(key_) {
		       i = value_;
		       value_ptr = &i;
	};
	FITSKeyValue(std::string key_, std::size_t value_) :
		type(TLONG), key(key_) {
			l = value_;
		        value_ptr = &l;
	};
	FITSKeyValue(std::string key_, long int value_) :
		type(TLONG), key(key_) {
			l = value_;	
		        value_ptr = &l;
	};
	FITSKeyValue(std::string key_, float value_) :
		type(TFLOAT), key(key_) {
			f = value_;	
		        value_ptr = &f;
	};
	FITSKeyValue(std::string key_, double value_) :
		type(TDOUBLE), key(key_) {
			d = value_;	
		        value_ptr = &d;
	};
	FITSKeyValue(std::string key_) : key(key_) { };
};

enum IOMode { read = READONLY, write = READWRITE };

class FITSFile {
	std::string filename;
	fitsfile *fptr = 0;
	int status = 0;
	int hduType = IMAGE_HDU; // IMAGE_HDU, ASCII_TBL, or BINARY_TBL
public:
	FITSFile(const std::string &filename_);
	~FITSFile();

	fitsfile * getPtr() { return fptr; }
	int getStatus() { return status; }

	void createFile();
	void deleteFile();
	void openFile(IOMode ioMode);

	void moveToHDU(int hduNumber);
	int getNumberOfHDUs();
	int getCurrentHDUNumber();

	void createImage(int bitpix, int naxis, long *naxes);
	void writeImage(int dtype, int firstElement,
			int nElements, void *array);
	std::unique_ptr<std::vector<float> > readImageAsFloat(int firstElement, int nElements);
	int getHDUType() { return hduType; }

	void createTable(int tableType, long int nRows, int nColumns, char *columnName[],
                char *columnType[], char *columnUnit[], const char *tableName);
	void writeDate();
	void writeColumn(int dataType, int column, long int firstRow, long int firstElement,
		long int nElements, void *array);

	void writeKeyValue(FITSKeyValue &kv, const char comment[]);
	
	FITSKeyValue readKeyValue(std::string key, int type);
	std::string readKeyValueAsString(std::string key) {
		return (readKeyValue(key, TSTRING)).getValueAsString();
	};
	int readKeyValueAsDouble(std::string key) {
		return (readKeyValue(key, TINT)).getValueAsDouble();
	};
	int readKeyValueAsInt(std::string key) {
		return (readKeyValue(key, TINT)).getValueAsInt();
	};
};

} // namespace hermes

#endif // HERMES_FITWRAPPER_H

#endif // HERMES_HAVE_CFITSIO

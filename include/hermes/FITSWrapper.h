#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_FITSWRAPPER_H
#define HERMES_FITSWRAPPER_H

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "fitsio.h"

namespace hermes {

namespace FITS {
// TODO: switch to enum class
enum DataType {
	INT = TINT,
	LONG = TLONG,
	FLOAT = TFLOAT,
	DOUBLE = TDOUBLE,
	STRING = TSTRING
};
enum IOMode { READ = READONLY, WRITE = READWRITE };
enum HDUType { IMAGE = IMAGE_HDU, ASCII = ASCII_TBL, BINARY = BINARY_TBL };
enum ImgType {
	IMGBYTE = BYTE_IMG,    /*  8-bit unsigned integers */
	IMGSHORT = SHORT_IMG,  /* 16-bit   signed integers */
	IMGLONG = LONG_IMG,    /* 32-bit   signed integers */
	IMGFLOAT = FLOAT_IMG,  /* 32-bit single precision floating point */
	IMGDOUBLE = DOUBLE_IMG /* 64-bit double precision floating point */
};
}  // namespace FITS

struct FITSKeyValue {
	std::string key;
	FITS::DataType type;
	// TODO: implement through std::variant, but it requires C++17
	union {
		int i;
		long int l;
		float f;
		double d;
		char s[80];
	};
	void *value_ptr;

  public:
	FITS::DataType getType() { return type; }
	void setType(FITS::DataType type_) { type = type_; }
	void setTypeAsInt(int type_) {
		switch (type) {
			case TINT:
				type = FITS::INT;
				break;
			case TLONG:
				type = FITS::LONG;
				break;
			case TFLOAT:
				type = FITS::FLOAT;
				break;
			case TDOUBLE:
				type = FITS::DOUBLE;
				break;
			case TSTRING:
				type = FITS::STRING;
				break;
		}
	}
	const char *getKey() { return &key[0]; }
	void *getValueAsVoid() { return value_ptr; }
	std::string getValueAsString() { return static_cast<std::string>(s); }
	int getValueAsInt() { return i; }
	double getValueAsDouble() { return d; }

	FITSKeyValue(const std::string &key_, const std::string &value_)
	    : type(FITS::STRING), key(key_) {
		// ugly?
		strncpy(s, value_.c_str(), sizeof(s));
		s[sizeof(s) - 1] = 0;
		value_ptr = static_cast<void *>(s);
	};
	FITSKeyValue(const std::string &key_, int value_)
	    : type(FITS::INT), key(key_) {
		i = value_;
		value_ptr = &i;
	};
	FITSKeyValue(const std::string &key_, std::size_t value_)
	    : type(FITS::LONG), key(key_) {
		l = value_;
		value_ptr = &l;
	};
	FITSKeyValue(const std::string &key_, long int value_)
	    : type(FITS::LONG), key(key_) {
		l = value_;
		value_ptr = &l;
	};
	FITSKeyValue(const std::string &key_, float value_)
	    : type(FITS::FLOAT), key(key_) {
		f = value_;
		value_ptr = &f;
	};
	FITSKeyValue(const std::string &key_, double value_)
	    : type(FITS::DOUBLE), key(key_) {
		d = value_;
		value_ptr = &d;
	};
	explicit FITSKeyValue(const std::string &key_) : key(key_){};
};

class FITSFile {
  private:
	std::string filename;
	fitsfile *fptr = 0;
	int status = 0;

	FITS::HDUType hduType = FITS::IMAGE;
	FITS::HDUType intToHDUType(int hduType_);

  public:
	explicit FITSFile(const std::string &filename_);
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

	FITSKeyValue readKeyValue(const std::string &key_, FITS::DataType type_);
	inline std::string readKeyValueAsString(const std::string &key_) {
		return (readKeyValue(key_, FITS::STRING)).getValueAsString();
	};
	inline double readKeyValueAsDouble(const std::string &key_) {
		return (readKeyValue(key_, FITS::DOUBLE)).getValueAsDouble();
	};
	inline int readKeyValueAsInt(const std::string &key_) {
		return (readKeyValue(key_, FITS::INT)).getValueAsInt();
	};

	/* Image-related operations */
	void createImage(FITS::ImgType bitpix, int naxis, long *naxes);
	void writeImage(FITS::DataType dtype, int firstElement, int nElements,
	                void *array);
	std::vector<float> readImageAsFloat(unsigned int firstElement,
	                                    unsigned int nElements);

	void createTable(FITS::HDUType, long int nRows, int nColumns,
	                 char *columnName[], char *columnType[], char *columnUnit[],
	                 const char *tableName);
	void writeDate();
	void writeColumn(FITS::DataType dataType, int column, long int firstRow,
	                 long int firstElement, long int nElements, void *array);
};

}  // namespace hermes

#endif  // HERMES_FITSWRAPPER_H

#endif  // HERMES_HAVE_CFITSIO

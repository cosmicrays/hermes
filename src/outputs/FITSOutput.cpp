#ifdef HERMES_HAVE_CFITSIO

#include "hermes/outputs/FITSOutput.h"

#include <iostream>

namespace hermes {

FITSOutput::FITSOutput(const std::string &filename) : Output(), filename(filename) {
}

int FITSOutput::create_file() {
	int status = 0;
	if (fits_create_file(&fptr, filename.c_str(), &status))
		fits_report_error(stderr, status);
	return status;
}

int FITSOutput::write_key(const char key[], int type, void *value, const char comment[]) {
	int status = 0;
	if (fits_write_key(fptr, type, key, value, comment, &status))
		fits_report_error(stderr, status);
	return status;
}

void FITSOutput::save(std::shared_ptr<Skymap> skymap) {
	int status = 0;
	int bitpix = FLOAT_IMG; // DOUBLE_IMG;
	int firstElem = 1;
	std::size_t npix = skymap->getNpix();
	std::size_t nside = skymap->getNside();
	std::size_t res = skymap->getRes();
	int naxis  = 1;
	long int nullnaxes[1] = {1};
	float nullArray[1] = {0};
	long int naxes[naxis] = {(long)npix};
	
	float tempArray[npix] = {0};

	for (unsigned long i = 0; i < npix; ++i)
		tempArray[i] = (float) skymap->operator[](i);

	
	//std::cout << tempArray[10] << std::endl;	


	status = create_file();
	
	if (fits_create_img(fptr, bitpix, 1, nullnaxes, &status))
		fits_report_error(stderr, status);
	
	if (fits_write_img(fptr, TFLOAT, 1, 1, nullArray, &status))
		fits_report_error(stderr, status);
	
	long tfields = 1;
	int nrows = (int) (npix);
	char* ttype[] = { (char*)("TFLOAT") };
	char* tform[] = { (char*)("1E") };
	char* tunit[] = { (char*)("nothing") };
	const char extname[] = "xtension";
	
	if (fits_create_tbl(fptr, BINARY_TBL, nrows, tfields, ttype, tform, tunit, extname, &status))


	if (fits_write_date(fptr, &status))
		fits_report_error(stderr, status);

	char type_str[] = "HEALPIX";
	status = write_key("PIXTYPE", TSTRING, type_str, "HEALPIX Pixelisation");
	char ext_str[] = "xtension";
	status = write_key("EXTNAME", TSTRING, ext_str, "Name of this binary table extension");
	char ordering[] = "RING    ";
	status = write_key("ORDERING", TSTRING, ordering, "Pixel ordering scheme, either RING or NESTED");
	status = write_key("NSIDE", TLONG, &nside, "Resolution parameter for HEALPIX");
	status = write_key("RES", TDOUBLE, &res, "Resolution of map");
	char object[] = "FULLSKY";
	status = write_key("OBJECT", TSTRING, object, "Sky coverage, either FULLSKY or PARTIAL");
	char indx_str[] = "IMPLICIT";
	status = write_key("INDXSCHM", TSTRING, indx_str, "Indexing: IMPLICIT or EXPLICIT");

	char process[] = "Rotation_measure";
	status = write_key("PROCESS", TSTRING, &process, NULL);
	
	//if (fits_write_img(fptr, TDOUBLE, firstElem, npix, tempArray, &status))
	//	fits_report_error(stderr, status);
	
	fits_write_col(fptr, TFLOAT, 1, 1, 1, npix, tempArray, &status);
		fits_report_error(stderr, status);

	fits_close_file(fptr, &status);
}

} // namespace hermes

#endif // HERMES_HAVE_CFITSIO

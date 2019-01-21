#ifdef HERMES_HAVE_CFITSIO

#include "hermes/outputs/FITSOutput.h"

#include <iostream>
#include <string>

namespace hermes {


FITSOutput::FITSOutput(const std::string &filename) : Output(), filename(filename) {
}

void FITSOutput::save(std::shared_ptr<Skymap> skymap) {
	auto ffile = std::make_unique<FITSFile>(FITSFile(filename));

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


	ffile->createFile();
	
	ffile->createImage(bitpix, 1, nullnaxes);
	ffile->writeImage(TFLOAT, 1, 1, nullArray);	
	
	long tfields = 1;
	int nrows = (int) (npix);
	char* ttype[] = { (char*)("TFLOAT") };
	char* tform[] = { (char*)("1E") };
	char* tunit[] = { (char*)("nothing") };
	const char extname[] = "xtension";

	ffile->createTable(BINARY_TBL, nrows, tfields, ttype, tform, tunit, extname);

	ffile->writeDate();

	auto str_type = FITSKeyValue("PIXTYPE", "HEALPIX");
	ffile->writeKeyValue(str_type, "HEALPIX Pixelisation");
	auto ext_str = FITSKeyValue("EXTNAME", "xtension");
	ffile->writeKeyValue(ext_str, "Name of this binary table extension");
	auto ordering = FITSKeyValue("ORDERING", "RING    ");
	ffile->writeKeyValue(ordering, "Pixel ordering scheme, either RING or NESTED");
	auto f_nside = FITSKeyValue("NSIDE", nside);
	ffile->writeKeyValue(f_nside, "Resolution parameter for HEALPIX");
	auto f_res = FITSKeyValue("RES", res);
	ffile->writeKeyValue(f_res, "Resolution of map");
	auto object = FITSKeyValue("OBJECT", "FULLSKY");
	ffile->writeKeyValue(object, "Sky coverage, either FULLSKY or PARTIAL");
	auto indx_str = FITSKeyValue("INDXSCHM", "IMPLICIT");
	ffile->writeKeyValue(indx_str, "Indexing: IMPLICIT or EXPLICIT");
	auto process = FITSKeyValue("PROCESS", "Rotation_measure");
	ffile->writeKeyValue(process, NULL);
	
	//if (fits_write_img(fptr, TDOUBLE, firstElem, npix, tempArray, &status))
	//	fits_report_error(stderr, status);
	
	ffile->writeColumn(TFLOAT, 1, 1, 1, npix, tempArray);
}

} // namespace hermes

#endif // HERMES_HAVE_CFITSIO

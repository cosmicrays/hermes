#include "gtest/gtest.h"

#include "hermes.h"
#include <memory>

namespace hermes {

TEST(FITS, testFITSKeyValue) {
	std::string str = "xtension";
	auto fstr = FITSKeyValue("EXTNAME", str);
	EXPECT_EQ(fstr.getType(), FITS::STRING);
	EXPECT_EQ(static_cast<std::string>(fstr.getKey()), "EXTNAME");
	EXPECT_EQ(fstr.getValueAsString(), str);

	int nside = 16;
	auto fint = FITSKeyValue("NSIDE", nside);
	EXPECT_EQ(fint.getType(), FITS::INT);
	EXPECT_EQ(static_cast<std::string>(fint.getKey()), "NSIDE");
	EXPECT_EQ(fint.getValueAsInt(), nside);
	
	double mypi = 3.14;
	auto fdouble = std::make_unique<FITSKeyValue>(FITSKeyValue("PI", mypi));
	EXPECT_EQ(fdouble->getType(), FITS::DOUBLE);
	EXPECT_EQ(static_cast<std::string>(fdouble->getKey()), "PI");
	EXPECT_EQ(fdouble->getValueAsDouble(), mypi);
}

TEST(FITS, createDeleteFile) {
	auto ffile = std::make_unique<FITSFile>(FITSFile("!testFITSNewFile.fits"));
	long int nullnaxes[1] = {1};
	
	ffile->createFile();
	ffile->createImage(FITS::IMGFLOAT, 1, nullnaxes);
	ffile->deleteFile();
}

TEST(FITS, setGetKey) {
	auto ffile = std::make_unique<FITSFile>(FITSFile("!testFITSNewFile.fits"));
	long int nullnaxes[1] = {1};
	float nullArray[1] = {0};

	// Create a file with some keywords	
	ffile->createFile();
	ffile->createImage(FITS::IMGFLOAT, 1, nullnaxes);
	ffile->writeImage(FITS::FLOAT, 1, 1, nullArray);
	
	long tfields = 1;
	int nrows = (int) (20);
	char* ttype[] = { (char*)("TFLOAT") };
	char* tform[] = { (char*)("1E") };
	char* tunit[] = { (char*)("nothing") };
	const char extname[] = "xtension";

	ffile->createTable(FITS::BINARY, nrows, tfields, ttype, tform, tunit, extname);
	ffile->writeDate();

	auto str_type = FITSKeyValue("PIXTYPE", "HEALPIX");
	ffile->writeKeyValue(str_type, "HEALPIX Pixelisation");
	auto ext_str = FITSKeyValue("EXTNAME", "xtension");
	ffile->writeKeyValue(ext_str, "Name of this binary table extension");
	auto ordering = FITSKeyValue("ORDERING", "RING    ");
	ffile->writeKeyValue(ordering, "Pixel ordering scheme, either RING or NESTED");
	auto f_nside = FITSKeyValue("NSIDE", 16);
	ffile->writeKeyValue(f_nside, "Resolution parameter for HEALPIX");
	
	ffile->closeFile();
	EXPECT_EQ(ffile->getStatus(), 0);

	// Open it again
	
	auto new_ffile = std::make_unique<FITSFile>(FITSFile("testFITSNewFile.fits"));
	new_ffile->openFile(FITS::READ);
	new_ffile->moveToHDU(2);
	
	EXPECT_EQ(new_ffile->getHDUType(), FITS::BINARY);

	std::string new_ext_str = new_ffile->readKeyValueAsString("EXTNAME");
	EXPECT_EQ(ext_str.getValueAsString(), new_ext_str);
	int new_f_nside = new_ffile->readKeyValueAsInt("NSIDE");
	EXPECT_EQ(f_nside.getValueAsInt(), new_f_nside);
	new_ffile->deleteFile();
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


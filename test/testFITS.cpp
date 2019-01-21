#include "gtest/gtest.h"

#include "hermes.h"
#include <memory>

namespace hermes {

TEST(FITS, testFITSValue) {
	std::string str = "xtension";
	auto fstr = FITSKeyValue("EXTNAME", str);
	EXPECT_EQ(fstr.getType(), TSTRING);
	EXPECT_EQ(static_cast<std::string>(fstr.getKey()), "EXTNAME");
	EXPECT_EQ(fstr.getValueAsString(), str);

	int nside = 16;
	auto fint = FITSKeyValue("NSIDE", nside);
	EXPECT_EQ(fint.getType(), TINT);
	EXPECT_EQ(static_cast<std::string>(fint.getKey()), "NSIDE");
	EXPECT_EQ(fint.getValueAsInt(), nside);
	
	double mypi = 3.14;
	auto fdouble = std::make_unique<FITSKeyValue>(FITSKeyValue("PI", mypi));
	EXPECT_EQ(fdouble->getType(), TDOUBLE);
	EXPECT_EQ(static_cast<std::string>(fdouble->getKey()), "PI");
	EXPECT_EQ(fdouble->getValueAsDouble(), mypi);
}

TEST(FITS, createDeleteFile) {
	auto ffile = std::make_unique<FITSFile>(FITSFile("!testFITSNewFile.fits"));
	long int nullnaxes[1] = {1};

	ffile->createFile();
	ffile->createImage(FLOAT_IMG, 1, nullnaxes);
	ffile->deleteFile();
}

TEST(FITS, setGetKey) {
	auto ffile = std::make_unique<FITSFile>(FITSFile("!testFITSNewFile.fits"));
	long int nullnaxes[1] = {1};

	ffile->createFile();
	ffile->createImage(FLOAT_IMG, 1, nullnaxes);
	auto kv = FITSKeyValue("EXTNAME", "xtension");
	ffile->writeKeyValue(kv, "Some comment");
	//auto str = ffile->readKeyValue("EXTNAME", TSTRING);
	//EXPECT_EQ(str.getValueAsString(), kv.getValueAsString());
	//ffile->deleteFile();
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


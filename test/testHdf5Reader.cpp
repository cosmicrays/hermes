#ifdef HERMES_HAVE_HDF5

#include <string>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

std::string testFilePath = getDataPath(
    "CosmicRays/Picard3DTest/Hydrogen_1.h5");

TEST(Hdf5ReaderTest, readAttributeFromDataGroup) {
	Hdf5Reader reader(testFilePath);

	std::string attributeName;

	int aOfParticle8{};
	attributeName = "A of particle 8";
	reader.readAttributeFromDataGroup("A of particle 8", aOfParticle8);
	EXPECT_EQ(aOfParticle8, 1)
	    << "Reading of the global attribute '" << attributeName << "' failed.";

	int entries{};
	attributeName = "Entries";
	reader.readAttributeFromDataGroup(attributeName, entries);
	EXPECT_EQ(entries, 31) << "Reading of the global attribute '"
	                       << attributeName << "' failed.";

	int kElectronsOfSpecies8{};
	attributeName = "K-electrons of species 8";
	reader.readAttributeFromDataGroup(attributeName, kElectronsOfSpecies8);
	EXPECT_EQ(kElectronsOfSpecies8, 0)
	    << "Reading of the global attribute '" << attributeName << "' failed.";

	std::string datasetName;
	attributeName = "Name_om00";
	reader.readAttributeFromDataGroup(attributeName, datasetName);
	EXPECT_EQ(datasetName, "Hydrogen_1_E00")
	    << "Reading of the global attribute '" << attributeName << "' failed.";

	int zOfParticle8{};
	attributeName = "Z of particle 8";
	reader.readAttributeFromDataGroup(attributeName, zOfParticle8);
	EXPECT_EQ(zOfParticle8, 1)
	    << "Reading of the global attribute '" << attributeName << "' failed.";

	double time{};
	attributeName = "time";
	reader.readAttributeFromDataGroup(attributeName, time);
	EXPECT_DOUBLE_EQ(time, 0.)
	    << "Reading of the global attribute '" << attributeName << "' failed.";

	int timestep{};
	attributeName = "timestep";
	reader.readAttributeFromDataGroup(attributeName, timestep);
	EXPECT_EQ(timestep, 0) << "Reading of the global attribute '"
	                       << attributeName << "' failed.";

	std::vector<double> yGridCentred;
	attributeName = "yGridCentred";
	reader.readAttributeFromDataGroup(attributeName, yGridCentred);
	std::vector<double> yGridCentredExpected{
	    -20,   -18.75, -17.5, -16.25, -15,   -13.75, -12.5, -11.25, -10,
	    -8.75, -7.5,   -6.25, -5,     -3.75, -2.5,   -1.25, 0,      1.25,
	    2.5,   3.75,   5,     6.25,   7.5,   8.75,   10,    11.25,  12.5,
	    13.75, 15,     16.25, 17.5,   18.75, 20};
	ASSERT_EQ(yGridCentred.size(), yGridCentredExpected.size())
	    << "Reading of the global attribute '" << attributeName
	    << "' failed: invalid vector size";
	for (size_t index = 0; index < yGridCentredExpected.size(); ++index) {
		EXPECT_DOUBLE_EQ(yGridCentred[index], yGridCentredExpected[index])
		    << "Reading of the global attribute '" << attributeName
		    << "' failed: expected " << yGridCentredExpected[index]
		    << " at index " << index << ". got " << yGridCentred[index] << ".";
	}
}

TEST(Hdf5ReaderTest, ReadAttributeFromDataset) {
	Hdf5Reader reader(testFilePath);
	std::string datasetName("Hydrogen_1_E01");
	std::string attributeName;

	double totalEnergy;
	attributeName = "Etot";
	reader.readAttributeFromDatasetOfDataGroup(datasetName, attributeName,
	                                           totalEnergy);
	EXPECT_DOUBLE_EQ(totalEnergy, 956.75049597422299)
	    << "Reading of the attribute '" << attributeName
	    << "' from the dataset '" << datasetName << "' failed.";
}

TEST(Hdf5ReaderTest, FindAttributeIndex) {
	Hdf5Reader reader(testFilePath);
	std::string partOfTheAttributeName;
	hid_t attributeIndex;

	int aOfParticle8;
	partOfTheAttributeName = "A of par";
	attributeIndex = reader.findAttributeIndex(partOfTheAttributeName);
	EXPECT_NE(attributeIndex, -1)
	    << "Finding the index of the attribute with '" << partOfTheAttributeName
	    << "' in the name failed.";
	reader.readAttributeFromDataGroup(attributeIndex, aOfParticle8);
	EXPECT_EQ(aOfParticle8, 1)
	    << "Finding the index of the attribute with '" << partOfTheAttributeName
	    << "' in the name failed.";

	int zOfParticle8;
	partOfTheAttributeName = "Z of p";
	attributeIndex = reader.findAttributeIndex(partOfTheAttributeName);
	EXPECT_NE(attributeIndex, -1)
	    << "Finding the index of the attribute with '" << partOfTheAttributeName
	    << "' in the name failed.";
	reader.readAttributeFromDataGroup(attributeIndex, zOfParticle8);
	EXPECT_EQ(zOfParticle8, 1)
	    << "Finding the index of the attribute with '" << partOfTheAttributeName
	    << "' in the name failed.";
}

size_t getIndex(std::size_t xIndex, std::size_t yIndex, std::size_t zIndex,
                const std::vector<int> &datasetDimensions) {
	int numberOfXValues(datasetDimensions[2]);
	int numberOfYValues(datasetDimensions[1]);
	return zIndex * numberOfYValues * numberOfXValues +
	       yIndex * numberOfXValues + xIndex;
}

TEST(Hdf5ReaderTest, ReadDataset) {
	Hdf5Reader reader(testFilePath);

	std::string datasetName("Hydrogen_1_E01");
	std::vector<float> datasetContent;
	std::vector<int> datasetDimensions{17, 33, 33};  // z, y, x
	reader.readDataset(datasetName, datasetDimensions, datasetContent);

	size_t index = getIndex(16, 14, 8, datasetDimensions);
	float datasetValue = datasetContent[index];
	EXPECT_FLOAT_EQ(datasetValue, 1.1305535)
	    << "Reading of the contents from the dataset '" << datasetName
	    << "' failed.";

	index = getIndex(13, 19, 8, datasetDimensions);
	datasetValue = datasetContent[index];
	EXPECT_FLOAT_EQ(datasetValue, 0.48015749)
	    << "Reading of the contents from the dataset '" << datasetName
	    << "' failed.";

	index = getIndex(12, 20, 3, datasetDimensions);
	datasetValue = datasetContent[index];
	EXPECT_FLOAT_EQ(datasetValue, 0.013993724)
	    << "Reading of the contents from the dataset '" << datasetName
	    << "' failed.";
}

}  // namespace hermes

#endif  // HERMES_HAVE_HDF5

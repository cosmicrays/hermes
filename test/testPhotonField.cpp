#include <cstdio>
#include <fstream>
#include <vector>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

TEST(CMB, getEnergyDensity) {
	auto field = std::make_shared<photonfields::CMB>(photonfields::CMB());
	/*
	    std::ofstream spectrumfile("isrf.txt");
	    for (QFrequency f = 1e10_Hz; f < 1e16_Hz; f = f*1.01) {
	        QEnergy photon_energy = f*h_planck;
	        spectrumfile << std::scientific << std::setprecision(3)
	   << f
	   << "\t" << photon_energy / 1_eV << "\t"; spectrumfile <<
	   field->getEnergyDensity(8.5_kpc, 0., photon_energy) / 1_eV * 1_cm3 <<
	   "\t"; spectrumfile << field->getEnergyDensity(4.0_kpc, 0.,
	   photon_energy) / 1_eV * 1_cm3 << "\t"; spectrumfile <<
	   field->getEnergyDensity(0.01_kpc, 0., photon_energy) / 1_eV * 1_cm3
	   << "\n";
	    }
	    spectrumfile.close();
	*/
}

TEST(ISRF, loadsRuntimeData) {
	auto photonField =
	    std::make_shared<photonfields::ISRF>(photonfields::ISRF());
	EXPECT_EQ(photonField->getSize(), 30 * 24 * 1211);

	const QEnergy photonEnergy =
	    h_planck * c_light / (std::pow(10.0, 1.01) * micrometre);
	const auto densityAtGridPoint =
	    photonField->getEnergyDensity(6.0_kpc, 25.0_kpc, photonEnergy) /
	    1_eV * 1_cm3;
	EXPECT_NEAR(static_cast<double>(densityAtGridPoint), 5.013677e-4,
	            1e-12);

	const auto interpolatedDensity =
	    photonField->getEnergyDensity(6.25_kpc, 27.5_kpc, photonEnergy) /
	    1_eV * 1_cm3;
	EXPECT_NEAR(static_cast<double>(interpolatedDensity), 3.90662975e-4,
	            1e-12);

	const auto boundaryDensity =
	    photonField->getEnergyDensity(30.0_kpc, 30.0_kpc, photonEnergy) /
	    1_eV * 1_cm3;
	EXPECT_NEAR(static_cast<double>(boundaryDensity), 1.137146e-4, 1e-12);
	EXPECT_EQ(photonField->getEnergyDensity(31.0_kpc, 0.0_kpc, photonEnergy),
	          QEnergyDensity(0));

	for (auto itE = photonField->begin(); itE != photonField->end(); ++itE) {
		// integral += crossSec->getDiffCrossSection(Eelectron_,
		// static_cast<double>(*itE), Egamma_) *
		//		phdensity->getEnergyDensity(pos_,
		// static_cast<int>(itE - phdensity->begin())) / (*itE);
	}
	/*
	    std::ofstream spectrumfile("isrf.txt");
	    for (QFrequency f = 1e10_Hz; f < 1e16_Hz; f = f*1.01) {
	        QEnergy photon_energy = f*h_planck;
	        spectrumfile << std::scientific << std::setprecision(3)
	   << f
	   << "\t" << photon_energy / 1_eV << "\t"; spectrumfile <<
	   field->getEnergyDensity(8.5_kpc, 0., photon_energy) / 1_eV * 1_cm3 <<
	   "\t"; spectrumfile << field->getEnergyDensity(4.0_kpc, 0.,
	   photon_energy) / 1_eV * 1_cm3 << "\t"; spectrumfile <<
	   field->getEnergyDensity(0.01_kpc, 0., photon_energy) / 1_eV * 1_cm3
	   << "\n";
	    }
	    spectrumfile.close();
	*/
}

TEST(ISRF, loadsCombinedFITSData) {
	const std::string filename = "testISRFDataCube.fits";
	const std::size_t nLambda = 1211;
	const std::size_t nZ = 24;
	const std::size_t nR = 30;
	std::vector<double> logWavelengths(nLambda);
	std::vector<double> radii(nR);
	std::vector<double> heights(nZ);
	std::vector<double> densities(nLambda * nZ * nR);
	for (std::size_t index = 0; index < nLambda; ++index)
		logWavelengths[index] = -2.0 + 0.005 * index;
	for (std::size_t index = 0; index < nR; ++index)
		radii[index] = static_cast<double>(index);
	for (std::size_t index = 0; index < nZ; ++index)
		heights[index] = static_cast<double>(index);
	for (std::size_t ir = 0; ir < nR; ++ir) {
		for (std::size_t iz = 0; iz < nZ; ++iz) {
			for (std::size_t iLambda = 0; iLambda < nLambda; ++iLambda) {
				const auto flatIndex =
				    iLambda + iz * nLambda + ir * nLambda * nZ;
				densities[flatIndex] =
				    100.0 * ir + 10.0 * iz + iLambda;
			}
		}
	}

	{
		FITSFile file("!" + filename);
		file.createFile();
		long cubeDimensions[3] = {static_cast<long>(nLambda),
		                          static_cast<long>(nZ),
		                          static_cast<long>(nR)};
		file.createImage(FITS::IMGDOUBLE, 3, cubeDimensions);
		file.writeImage(FITS::DOUBLE, 1, static_cast<int>(densities.size()),
		                densities.data());

		const auto writeAxis = [&file](const std::string &name,
		                                   std::vector<double> &axis) {
			long dimensions[1] = {static_cast<long>(axis.size())};
			file.createImage(FITS::IMGDOUBLE, 1, dimensions);
			FITSKeyValue extensionName("EXTNAME", name);
			file.writeKeyValue(extensionName, "axis name");
			file.writeImage(FITS::DOUBLE, 1, static_cast<int>(axis.size()),
			                axis.data());
		};
		writeAxis("LOGWAVE", logWavelengths);
		writeAxis("R_GRID", radii);
		writeAxis("Z_GRID", heights);
		file.closeFile();
	}

	{
		photonfields::ISRF field(filename);
		EXPECT_EQ(field.getSize(), densities.size());
		const std::size_t wavelengthIndex = 604;
		const QEnergy photonEnergy =
		    h_planck * c_light /
		    (std::pow(10.0, logWavelengths[wavelengthIndex]) * micrometre);
		const auto density =
		    field.getEnergyDensity(2.0_kpc, 3.0_kpc, photonEnergy) /
		    1_eV * 1_cm3;
		EXPECT_NEAR(static_cast<double>(density), 834.0, 1e-8);
	}
	EXPECT_EQ(std::remove(filename.c_str()), 0);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

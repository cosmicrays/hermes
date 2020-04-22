#include "gtest/gtest.h"

#include "hermes.h"
#include <fstream>

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

TEST(CMB, ISRF) {
	auto photonField =
	    std::make_shared<photonfields::ISRF>(photonfields::ISRF());

	for (auto itE = photonField->begin(); itE != photonField->end();
	     ++itE) {
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

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

} // namespace hermes

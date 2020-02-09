#include "gtest/gtest.h"

#include "hermes.h"
#include <fstream>

namespace hermes {

/*
	Example 1 from cparamlib
	total inclusive gamma-ray cross section
	Tp = 512000.00 GeV
	E = 100.00 GeV
	=> dsigma/dlogE = 179.5 mb
*/
TEST(Interactions, cparamlib) {

	auto interaction = std::make_shared<Kamae06>(Kamae06());

	QEnergy E_p = 512000_GeV;
	QEnergy E_gamma = 100_GeV;
	QDiffCrossSection dsigma_dE = interaction->getDiffCrossSectionDirectly(E_p, E_gamma);
	
	QArea r = dsigma_dE * E_gamma;

	EXPECT_NEAR(
		static_cast<double>(r),
		static_cast<double>(179.5_mbarn),
		static_cast<double>(0.1_mbarn));
}

TEST(Interactions, KleinNishina) {

	auto interaction = std::make_shared<KleinNishina>(KleinNishina());

	// low-energy limit
	QEnergy Egamma = 0.0001_eV;
	EXPECT_NEAR(static_cast<double>(
			interaction->getDiffCrossSection(m_electron*c_squared, Egamma, Egamma)),
		    static_cast<double>(3*(9-4*log(4))/32. * sigma_Thompson / Egamma),
		    1e-10);
	
	// high-energy zero
	QEnergy E_fast_electron = 1e10*m_electron*c_squared;
	EXPECT_NEAR(static_cast<double>(
			interaction->getDiffCrossSection(E_fast_electron, Egamma, 0.1*E_fast_electron)),
		    0.0,
		    1e-25);

	QEnergy E_photon =  6.626e-4_eV;
	//QEnergy E_gamma = 1e5_erg;
	QEnergy E_electron = 1e5*m_electron*c_squared;

	/*	
    	std::ofstream emissivityfile( "emissivity.txt" );
	for (auto E_gamma = 0.01_GeV; E_gamma < 100_GeV; E_gamma = E_gamma*1.1) {
        	emissivityfile << std::scientific << std::setprecision(3) << E_gamma / 1_GeV << "\t";
        	emissivityfile << E_gamma/1_erg *  interaction->getDiffCrossSection(E_electron, 1_eV, E_gamma)*1_erg/1_cm2 << "\t";
       		emissivityfile << std::endl;
    	}
    	emissivityfile.close();
	*/
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


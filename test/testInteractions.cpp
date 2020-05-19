#include <fstream>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

/*
    Example 1 from cparamlib
    total inclusive gamma-ray cross section
    Tp = 512000.00 GeV
    E = 100.00 GeV
    => dsigma/dlogE = 179.5 mb
*/
TEST(Interactions, Kamae06Gamma) {
	auto interaction = std::make_shared<interactions::Kamae06Gamma>(
	    interactions::Kamae06Gamma());

	QEnergy E_p = 512000_GeV;
	QEnergy E_gamma = 100_GeV;
	QDiffCrossSection dsigma_dE =
	    interaction->getDiffCrossSectionDirectly(E_p, E_gamma);

	QArea r = dsigma_dE * E_gamma;

	EXPECT_NEAR(static_cast<double>(r), static_cast<double>(179.5_mbarn),
	            static_cast<double>(0.1_mbarn));
}

TEST(Interactions, KelnerAharonianSigmaInelastic) {
	// based values from Fig 1 in astro-ph/1406.7369

	auto E_proton = 1e-1_TeV;
	auto sigma = interactions::KelnerAharonianGamma::sigmaInelastic(E_proton);
	EXPECT_NEAR(static_cast<double>(sigma), static_cast<double>(30_mbarn),
	            static_cast<double>(2_mbarn));

	E_proton = 20_TeV;
	sigma = interactions::KelnerAharonianGamma::sigmaInelastic(E_proton);
	EXPECT_NEAR(static_cast<double>(sigma), static_cast<double>(41_mbarn),
	            static_cast<double>(2_mbarn));

	E_proton = 1e-1_GeV;
	sigma = interactions::KelnerAharonianGamma::sigmaInelastic(E_proton);
	EXPECT_LE(static_cast<double>(sigma), static_cast<double>(1_mbarn));

	E_proton = 1e7_GeV;
	sigma = interactions::KelnerAharonianGamma::sigmaInelastic(E_proton);
	EXPECT_GE(static_cast<double>(sigma), static_cast<double>(50_mbarn));
}

TEST(Interactions, KelnerAharonianVsKamae06Gamma) {
	auto kamae06 = std::make_shared<interactions::Kamae06Gamma>(
	    interactions::Kamae06Gamma());
	auto kelahar = std::make_shared<interactions::KelnerAharonianGamma>(
	    interactions::KelnerAharonianGamma());

	auto E_proton = 100_GeV;
	auto E_gamma = 1_GeV;
	EXPECT_NEAR(
	    static_cast<double>(kamae06->getDiffCrossSection(E_proton, E_gamma)),
	    static_cast<double>(kelahar->getDiffCrossSection(E_proton, E_gamma)),
	    static_cast<double>(5e-21));

	E_proton = 1_TeV;
	E_gamma = 10_GeV;
	EXPECT_NEAR(
	    static_cast<double>(kamae06->getDiffCrossSection(E_proton, E_gamma)),
	    static_cast<double>(kelahar->getDiffCrossSection(E_proton, E_gamma)),
	    static_cast<double>(5e-21));
}

TEST(Interactions, KelnerAharonianVsKamae06Neutrino) {
	auto kamae06 = std::make_shared<interactions::Kamae06Neutrino>(
	    interactions::Kamae06Neutrino());
	auto kelahar = std::make_shared<interactions::KelnerAharonianNeutrino>(
	    interactions::KelnerAharonianNeutrino());

	auto E_proton = 10_TeV;
	auto E_neutrino = 1_TeV;
	EXPECT_NEAR(
	    static_cast<double>(kamae06->getDiffCrossSection(E_proton, E_neutrino)),
	    static_cast<double>(kelahar->getDiffCrossSection(E_proton, E_neutrino)),
	    static_cast<double>(5e-21));

	E_proton = 1_TeV;
	E_neutrino = 10_GeV;
	EXPECT_NEAR(
	    static_cast<double>(kamae06->getDiffCrossSection(E_proton, E_neutrino)),
	    static_cast<double>(kelahar->getDiffCrossSection(E_proton, E_neutrino)),
	    static_cast<double>(5e-21));
}

TEST(Interactions, KleinNishina) {
	auto interaction = std::make_shared<interactions::KleinNishina>(
	    interactions::KleinNishina());

	// low-energy gamma limit
	double lorentz = 1e10;
	QEnergy Eelectron = lorentz * m_electron * c_squared;
	QEnergy Ephoton = 1.0e-3_eV;
	QEnergy Egamma = 1.0_eV;

	EXPECT_NEAR(static_cast<double>(interaction->getDiffCrossSection(
	                Eelectron, Ephoton, Egamma)),
	            static_cast<double>(3 / 4. * sigma_Thompson / Ephoton /
	                                (lorentz * lorentz)),
	            1e-30);

	// high-energy gamma -> zero
	EXPECT_NEAR(static_cast<double>(interaction->getDiffCrossSection(
	                Eelectron, Ephoton, 0.1 * Eelectron)),
	            0.0, 1e-25);
}

TEST(Interactions, BremsstrahlungSimple) {
	auto i = std::make_shared<interactions::BremsstrahlungSimple>(
	    		interactions::BremsstrahlungSimple());
	auto t = interactions::BremsstrahlungSimple::Target::HI;
	i->disableCaching();

	// tabular data
	auto Eelectron = 10_MeV;
	auto Egamma = 0.01_MeV;
	auto res = Egamma * i->getDiffCrossSectionForTarget(t,
                        Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 35.821, 1e-1);
	
	Eelectron = 1e-1_MeV;
	Egamma = 0.037975_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t,
                        Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 20.0682, 1e-1);
	
	Eelectron = 1e-1_MeV;
	Egamma = 0.089543_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t,
                        Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 5.9469, 1e-1);
	
	i->enableCaching();
	
	Eelectron = 100_MeV;
	Egamma = 2.07965_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t,
                        Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 35.0416, 1e-1);
	
	Eelectron = 1e4_GeV;
	Egamma = 0.01_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t,
                        Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 35.922, 1e-1);
	
	Eelectron = 1e3_GeV;
	Egamma = 974514.0_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t,
                        Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 26.1198, 1e-2);
	
	Eelectron = 1e4_GeV;
	Egamma = 9131.6_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t,
                        Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 35.8892, 1e-1);
}


int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

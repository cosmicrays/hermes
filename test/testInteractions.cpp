#include <fstream>
#include <type_traits>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

namespace {

class InspectableAAfrag final : public interactions::AAfragXsecs {
  public:
	InspectableAAfrag() : AAfragXsecs(DEFAULT_GAMMASPECTRUM_FILE) {}

	double tableValue(const PID &projectile, const PID &target,
	                  std::size_t ix, std::size_t iy) const {
		const std::vector<double> *table = nullptr;
		if (projectile == Proton && target == Proton)
			table = &xs_pp;
		else if (projectile == Proton && target == Helium)
			table = &xs_pHe;
		else if (projectile == Helium && target == Proton)
			table = &xs_Hep;
		else if (projectile == Helium && target == Helium)
			table = &xs_HeHe;
		else
			throw std::invalid_argument("unsupported AAfrag particle pair");
		return table->at(ix * 768 + iy);
	}
};

InspectableAAfrag &inspectableAAfrag() {
	static InspectableAAfrag interaction;
	return interaction;
}

QEnergy projectileEnergyAt(double gridIndex) {
	return std::pow(10.0, gridIndex * 8.0 / 1023.0) * 1_GeV;
}

QEnergy secondaryEnergyAt(const QEnergy &projectile, double gridIndex) {
	const double logEta = -5.0 + gridIndex * 5.0 / 767.0;
	return projectile * std::pow(10.0, logEta);
}

void expectRelativeNear(QDiffCrossSection actual, QDiffCrossSection expected,
	                    double tolerance = 1e-10) {
	ASSERT_NE(expected, QDiffCrossSection(0));
	EXPECT_NEAR(static_cast<double>(actual / expected), 1.0, tolerance);
}

}  // namespace

static_assert(std::has_virtual_destructor<interactions::DifferentialCrossSection>::value,
              "DifferentialCrossSection must be safely destructible through its base class");

/*
    Example 1 from cparamlib
    total inclusive gamma-ray cross section
    Tp = 512000.00 GeV
    E = 100.00 GeV
    => dsigma/dlogE = 179.5 mb
*/
TEST(Interactions, Kamae06Gamma) {
	auto interaction = std::make_shared<interactions::Kamae06Gamma>(interactions::Kamae06Gamma());

	QEnergy E_p = 512000_GeV;
	QEnergy E_gamma = 100_GeV;
	QDiffCrossSection dsigma_dE = interaction->getDiffCrossSectionDirectly(E_p, E_gamma);

	QArea r = dsigma_dE * E_gamma;

	EXPECT_NEAR(static_cast<double>(r), static_cast<double>(179.5_mbarn), static_cast<double>(0.1_mbarn));
}

TEST(Interactions, KelnerAharonianSigmaInelastic) {
	// based values from Fig 1 in astro-ph/1406.7369

	auto E_proton = 1e-1_TeV;
	auto sigma = interactions::KelnerAharonianGamma::sigmaInelastic(E_proton);
	EXPECT_NEAR(static_cast<double>(sigma), static_cast<double>(30_mbarn), static_cast<double>(2_mbarn));

	E_proton = 20_TeV;
	sigma = interactions::KelnerAharonianGamma::sigmaInelastic(E_proton);
	EXPECT_NEAR(static_cast<double>(sigma), static_cast<double>(41_mbarn), static_cast<double>(2_mbarn));

	E_proton = 1e-1_GeV;
	sigma = interactions::KelnerAharonianGamma::sigmaInelastic(E_proton);
	EXPECT_LE(static_cast<double>(sigma), static_cast<double>(1_mbarn));

	E_proton = 1e7_GeV;
	sigma = interactions::KelnerAharonianGamma::sigmaInelastic(E_proton);
	EXPECT_GE(static_cast<double>(sigma), static_cast<double>(50_mbarn));
}

TEST(Interactions, KelnerAharonianVsKamae06Gamma) {
	auto kamae06 = std::make_shared<interactions::Kamae06Gamma>(interactions::Kamae06Gamma());
	auto kelahar = std::make_shared<interactions::KelnerAharonianGamma>(interactions::KelnerAharonianGamma());

	auto E_proton = 100_GeV;
	auto E_gamma = 1_GeV;
	EXPECT_NEAR(static_cast<double>(kamae06->getDiffCrossSection(E_proton, E_gamma)),
	            static_cast<double>(kelahar->getDiffCrossSection(E_proton, E_gamma)), static_cast<double>(5e-21));

	E_proton = 1_TeV;
	E_gamma = 10_GeV;
	EXPECT_NEAR(static_cast<double>(kamae06->getDiffCrossSection(E_proton, E_gamma)),
	            static_cast<double>(kelahar->getDiffCrossSection(E_proton, E_gamma)), static_cast<double>(5e-21));
}

TEST(Interactions, KelnerAharonianVsKamae06Neutrino) {
	auto kamae06 = std::make_shared<interactions::Kamae06Neutrino>(interactions::Kamae06Neutrino());
	auto kelahar = std::make_shared<interactions::KelnerAharonianNeutrino>(interactions::KelnerAharonianNeutrino());

	auto E_proton = 10_TeV;
	auto E_neutrino = 1_TeV;
	EXPECT_NEAR(static_cast<double>(kamae06->getDiffCrossSection(E_proton, E_neutrino)),
	            static_cast<double>(kelahar->getDiffCrossSection(E_proton, E_neutrino)), static_cast<double>(5e-21));

	E_proton = 1_TeV;
	E_neutrino = 10_GeV;
	EXPECT_NEAR(static_cast<double>(kamae06->getDiffCrossSection(E_proton, E_neutrino)),
	            static_cast<double>(kelahar->getDiffCrossSection(E_proton, E_neutrino)), static_cast<double>(5e-21));
}

TEST(Interactions, AAfrag) {
	auto aafraggamma = std::make_shared<interactions::AAfragGamma>(interactions::AAfragGamma());
	auto aafragnu = std::make_shared<interactions::AAfragNeutrino>(interactions::AAfragNeutrino());
	PID Proton(1, 1);

	auto E_proton = 10_TeV;
	auto E_secondary = 1_TeV;
	auto sigma = aafraggamma->getDiffCrossSection(Proton, Proton, E_proton, E_secondary);
	EXPECT_NEAR(static_cast<double>(sigma / (1_mbarn / 1_GeV)), 0.020134182, 1e-8);
	sigma = aafragnu->getDiffCrossSection(Proton, Proton, E_proton, E_secondary);
	EXPECT_NEAR(static_cast<double>(sigma / (1_mbarn / 1_GeV)), 0.021306042, 1e-8);

	E_proton = 1_TeV;
	E_secondary = 10_GeV;
	sigma = aafraggamma->getDiffCrossSection(Proton, Proton, E_proton, E_secondary);
	EXPECT_NEAR(static_cast<double>(sigma / (1_mbarn / 1_GeV)), 7.055951825, 1e-7);
	sigma = aafragnu->getDiffCrossSection(Proton, Proton, E_proton, E_secondary);
	EXPECT_NEAR(static_cast<double>(sigma / (1_mbarn / 1_GeV)), 14.74918925, 1e-7);
}

TEST(Interactions, AAfragReturnsExactGridNodesForAllNuclearPairs) {
	auto &interaction = inspectableAAfrag();
	constexpr std::size_t ix = 512;
	constexpr std::size_t iy = 500;
	const QEnergy projectile = projectileEnergyAt(ix);
	const QEnergy secondary = secondaryEnergyAt(projectile, iy);

	for (const auto &pair : std::vector<std::pair<PID, PID>>{
	         {Proton, Proton}, {Proton, Helium}, {Helium, Proton}, {Helium, Helium}}) {
		const QDiffCrossSection expected =
		    interaction.tableValue(pair.first, pair.second, ix, iy) * 1_mbarn / 1_GeV;
		expectRelativeNear(interaction.getDiffCrossSection(
		                       pair.first, pair.second, projectile, secondary),
		                   expected);
	}
}

TEST(Interactions, AAfragBilinearInterpolationUsesLocalCellFractions) {
	auto &interaction = inspectableAAfrag();
	constexpr std::size_t ix = 512;
	constexpr std::size_t iy = 500;
	const QEnergy projectile = projectileEnergyAt(ix + 0.5);
	const QEnergy secondary = secondaryEnergyAt(projectile, iy + 0.5);
	const double expectedRaw =
	    (interaction.tableValue(Proton, Proton, ix, iy) +
	     interaction.tableValue(Proton, Proton, ix + 1, iy) +
	     interaction.tableValue(Proton, Proton, ix, iy + 1) +
	     interaction.tableValue(Proton, Proton, ix + 1, iy + 1)) /
	    4.0;

	expectRelativeNear(
	    interaction.getDiffCrossSection(Proton, Proton, projectile, secondary),
	    expectedRaw * 1_mbarn / 1_GeV);
}

TEST(Interactions, AAfragHandlesDomainBoundariesAndTwoArgumentAPI) {
	auto &interaction = inspectableAAfrag();
	const QEnergy maxProjectile = projectileEnergyAt(1023);
	const QEnergy secondary = secondaryEnergyAt(maxProjectile, 500);
	const QDiffCrossSection expected =
	    interaction.tableValue(Proton, Proton, 1023, 500) * 1_mbarn / 1_GeV;
	expectRelativeNear(
	    interaction.getDiffCrossSection(Proton, Proton, maxProjectile, secondary),
	    expected);

	const QEnergy projectile = 10_TeV;
	const QEnergy gamma = 1_TeV;
	EXPECT_EQ(interaction.getDiffCrossSection(projectile, gamma),
	          interaction.getDiffCrossSection(Proton, Proton, projectile, gamma));
	EXPECT_EQ(interaction.getDiffCrossSection(projectile, QEnergy(0)),
	          QDiffCrossSection(0));
	EXPECT_EQ(interaction.getDiffCrossSection(projectile, 2 * projectile),
	          QDiffCrossSection(0));
}

TEST(Interactions, KleinNishina) {
	auto interaction = std::make_shared<interactions::KleinNishina>(interactions::KleinNishina());

	// low-energy gamma limit
	double lorentz = 1e10;
	QEnergy Eelectron = lorentz * m_electron * c_squared;
	QEnergy Ephoton = 1.0e-3_eV;
	QEnergy Egamma = 1.0_eV;

	EXPECT_NEAR(static_cast<double>(interaction->getDiffCrossSection(Eelectron, Ephoton, Egamma)),
	            static_cast<double>(3 / 4. * sigma_Thompson / Ephoton / (lorentz * lorentz)), 1e-30);

	// high-energy gamma -> zero
	EXPECT_NEAR(static_cast<double>(interaction->getDiffCrossSection(Eelectron, Ephoton, 0.1 * Eelectron)), 0.0, 1e-25);
}

TEST(Interactions, BremsstrahlungGALPROP) {
	auto i = std::make_shared<interactions::BremsstrahlungGALPROP>(interactions::BremsstrahlungGALPROP());
	auto t = interactions::BremsstrahlungAbstract::Target::HI;
	i->disableCaching();

	// tabular data
	auto Eelectron = 10_MeV;
	auto Egamma = 0.01_MeV;
	auto res = Egamma * i->getDiffCrossSectionForTarget(t, Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 35.821, 1e-1);

	Eelectron = 1e-1_MeV;
	Egamma = 0.037975_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t, Eelectron, Egamma) / 1_mbarn;
	// EXPECT_NEAR(static_cast<double>(res), 20.0682, 1e-1);

	Eelectron = 1e-1_MeV;
	Egamma = 0.089543_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t, Eelectron, Egamma) / 1_mbarn;
	// EXPECT_NEAR(static_cast<double>(res), 5.9469, 1e-1);

	i->enableCaching();

	Eelectron = 100_MeV;
	Egamma = 2.07965_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t, Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 35.0416, 1e-1);

	Eelectron = 1e4_GeV;
	Egamma = 0.01_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t, Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 35.922, 1e-1);

	Eelectron = 1e3_GeV;
	Egamma = 974514.0_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t, Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 26.1198, 1e-2);

	Eelectron = 1e4_GeV;
	Egamma = 9131.6_MeV;
	res = Egamma * i->getDiffCrossSectionForTarget(t, Eelectron, Egamma) / 1_mbarn;
	EXPECT_NEAR(static_cast<double>(res), 35.8892, 1e-1);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

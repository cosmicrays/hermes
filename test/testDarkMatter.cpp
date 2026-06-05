#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <memory>
#include <vector>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

namespace {

std::string writeHDMSpectraTestTable() {
	const std::string filename = "testHDMSpectraSpectrum.dat";
	std::ofstream output(filename.c_str());
	output << "mDM_GeV Log[10,x] q b gamma\n";
	output << "1.00000000e+08 -6 0 2.302585092994046 0\n";
	output << "1.00000000e+08 -5 0 4.605170185988092 0\n";
	output << "1.00000000e+10 -6 0 6.907755278982137 0\n";
	output << "1.00000000e+10 -5 0 9.210340371976184 0\n";
	return filename;
}

std::string writeTextFile(const std::string &filename, const std::string &contents) {
	std::ofstream output(filename.c_str());
	output << contents;
	return filename;
}

class ConstantDarkMatterSpectrum : public darkmatter::DarkMatterSpectrum {
  public:
	QEnergy getRestMassEnergy() const override { return 1e12_GeV; }
	QInverseEnergy getParticlesPerEnergy(QEnergy) const override { return 1. / 1_GeV; }
};

class ConstantGalacticProfile : public darkmatter::GalacticProfile {
  public:
	QMassDensity getMassDensity(QLength) const override { return 1e-21 * 1_kg / 1_m3; }
};

}  // namespace

TEST(DarkMatter, PPPC4DMIDSpectrum) {
	auto dmSpectrum = std::make_shared<darkmatter::PPPC4DMIDSpectrum>(
	    darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu, darkmatter::Mass::m100GeV));
	auto Egamma = 10_GeV;

	auto m = dmSpectrum->getRestMassEnergy();
	EXPECT_FLOAT_EQ(static_cast<double>(m), static_cast<double>(100_GeV));

	auto s = dmSpectrum->getParticlesPerEnergy(Egamma) * 1_erg;
	EXPECT_NEAR(static_cast<double>(s), 3.66656, 1e-4);
}

TEST(DarkMatter, PPPC4DMIDSpectrumRejectsUnphysicalEnergies) {
	auto dmSpectrum = std::make_shared<darkmatter::PPPC4DMIDSpectrum>(
	    darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu, darkmatter::Mass::m100GeV));

	EXPECT_EQ(dmSpectrum->getParticlesPerEnergy(0_GeV), QInverseEnergy(0));
	EXPECT_EQ(dmSpectrum->getParticlesPerEnergy(-1_GeV), QInverseEnergy(0));
	EXPECT_EQ(dmSpectrum->getParticlesPerEnergy(101_GeV), QInverseEnergy(0));
	EXPECT_TRUE(std::isfinite(static_cast<double>(dmSpectrum->getParticlesPerEnergy(10_GeV))));
}

TEST(DarkMatter, PPPC4DMIDSpectrumRejectsMissingFile) {
	EXPECT_THROW(darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu, darkmatter::Mass::m100GeV,
	                                           "missing-pppc4dmid-table.dat.gz"),
	             std::runtime_error);
}

TEST(DarkMatter, HDMSpectraSpectrumReadsExactMass) {
	const auto filename = writeHDMSpectraTestTable();
	darkmatter::HDMSpectraSpectrum dmSpectrum(darkmatter::Channel::b, 1e17_eV, darkmatter::HDMSpectraProduct::gamma,
	                                          filename);

	EXPECT_FLOAT_EQ(static_cast<double>(dmSpectrum.getRestMassEnergy()), static_cast<double>(1e17_eV));

	const auto s = dmSpectrum.getParticlesPerEnergy(50_GeV) * 1_GeV;
	EXPECT_NEAR(static_cast<double>(s), 1. / 50., 1e-12);
	std::remove(filename.c_str());
}

TEST(DarkMatter, HDMSpectraSpectrumInterpolatesMassAndEnergy) {
	const auto filename = writeHDMSpectraTestTable();
	darkmatter::HDMSpectraSpectrum massInterpolated(darkmatter::Channel::b, 1e18_eV,
	                                                darkmatter::HDMSpectraProduct::gamma, filename);
	const auto massInterpolatedValue = massInterpolated.getParticlesPerEnergy(500_GeV) * 1_GeV;
	EXPECT_NEAR(static_cast<double>(massInterpolatedValue), 2. / 500., 1e-12);

	darkmatter::HDMSpectraSpectrum energyInterpolated(darkmatter::Channel::b, 1e17_eV,
	                                                  darkmatter::HDMSpectraProduct::gamma, filename);
	const auto energy = 0.5 * 1e8 * std::pow(10., -5.5) * 1_GeV;
	const auto energyInterpolatedValue = energyInterpolated.getParticlesPerEnergy(energy) * 1_GeV;
	EXPECT_NEAR(static_cast<double>(energyInterpolatedValue), 1.5 / static_cast<double>(energy / 1_GeV), 1e-12);
	std::remove(filename.c_str());
}

TEST(DarkMatter, HDMSpectraSpectrumRejectsOutOfRangeInputs) {
	const auto filename = writeHDMSpectraTestTable();
	darkmatter::HDMSpectraSpectrum dmSpectrum(darkmatter::Channel::b, 1e17_eV, darkmatter::HDMSpectraProduct::gamma,
	                                          filename);

	EXPECT_EQ(dmSpectrum.getParticlesPerEnergy(0_GeV), QInverseEnergy(0));
	EXPECT_EQ(dmSpectrum.getParticlesPerEnergy(-1_GeV), QInverseEnergy(0));
	EXPECT_EQ(dmSpectrum.getParticlesPerEnergy(0.6 * 1e17_eV), QInverseEnergy(0));
	EXPECT_THROW(
	    darkmatter::HDMSpectraSpectrum(darkmatter::Channel::b, 1e16_eV, darkmatter::HDMSpectraProduct::gamma, filename),
	    std::invalid_argument);
	EXPECT_THROW(
	    darkmatter::HDMSpectraSpectrum(darkmatter::Channel::b, 1e25_eV, darkmatter::HDMSpectraProduct::gamma, filename),
	    std::invalid_argument);
	std::remove(filename.c_str());
}

TEST(DarkMatter, HDMSpectraSpectrumRejectsMalformedTables) {
	const auto missingChannel = writeTextFile("testHDMSpectraMissingChannel.dat",
	                                          "mDM_GeV Log[10,x] q gamma\n"
	                                          "1.00000000e+08 -6 0 0\n");
	EXPECT_THROW(darkmatter::HDMSpectraSpectrum(darkmatter::Channel::b, 1e17_eV, darkmatter::HDMSpectraProduct::gamma,
	                                            missingChannel),
	             std::runtime_error);
	std::remove(missingChannel.c_str());

	const auto inconsistentGrid = writeTextFile("testHDMSpectraInconsistentGrid.dat",
	                                            "mDM_GeV Log[10,x] b gamma\n"
	                                            "1.00000000e+08 -6 1 0\n"
	                                            "1.00000000e+08 -5 2 0\n"
	                                            "1.00000000e+10 -6 3 0\n");
	EXPECT_THROW(darkmatter::HDMSpectraSpectrum(darkmatter::Channel::b, 1e18_eV, darkmatter::HDMSpectraProduct::gamma,
	                                            inconsistentGrid),
	             std::runtime_error);
	std::remove(inconsistentGrid.c_str());

	const auto invalidValue = writeTextFile("testHDMSpectraInvalidValue.dat",
	                                        "mDM_GeV Log[10,x] b gamma\n"
	                                        "1.00000000e+08 -6 -1 0\n");
	EXPECT_THROW(darkmatter::HDMSpectraSpectrum(darkmatter::Channel::b, 1e17_eV, darkmatter::HDMSpectraProduct::gamma,
	                                            invalidValue),
	             std::runtime_error);
	std::remove(invalidValue.c_str());
}

TEST(DarkMatter, NFWGProfile) {
	QMass M_200 = 0.7 * 8e11 * sun_mass;  // Battaglia et al., 2005, MNRAS, 364 (converted to M_200)
	double concentration = 18;            // Battaglia et al., 2005, MNRAS, 364
	double gamma_slope = 1.;              // NFW
	auto dmProfile = std::make_shared<darkmatter::NFWGProfile>(gamma_slope, concentration, M_200);

	Vector3QLength r{8.1_kpc, 0, 0};
	auto rho = dmProfile->getMassDensity(r.getR());
	EXPECT_NEAR(static_cast<double>(rho), 6.0315e-22, 1e-24);
}

TEST(DarkMatter, NFWGProfileRejectsInvalidParameters) {
	const auto mass = 0.7 * 8e11 * sun_mass;

	EXPECT_THROW(darkmatter::NFWGProfile(-0.1, 18., mass), std::invalid_argument);
	EXPECT_THROW(darkmatter::NFWGProfile(2., 18., mass), std::invalid_argument);
	EXPECT_THROW(darkmatter::NFWGProfile(1., 0., mass), std::invalid_argument);
	EXPECT_THROW(darkmatter::NFWGProfile(1., -1., mass), std::invalid_argument);
	EXPECT_THROW(darkmatter::NFWGProfile(1., 18., -1. * sun_mass), std::invalid_argument);
}

TEST(DarkMatterIntegrator, spectralEmissivity) {
	auto dmSpectrum = std::make_shared<darkmatter::PPPC4DMIDSpectrum>(
	    darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu, darkmatter::Mass::m100GeV));

	QMass M_200 = 0.7 * 8e11 * sun_mass;  // Battaglia et al., 2005, MNRAS, 364 (converted to M_200)
	double concentration = 18;            // Battaglia et al., 2005, MNRAS, 364
	double gamma_slope = 1.;              // NFW
	auto dmProfile = std::make_shared<darkmatter::NFWGProfile>(gamma_slope, concentration, M_200);

	auto i = std::make_shared<DarkMatterIntegrator>(dmSpectrum, dmProfile);

	auto units = 1. / 1_GeV / 1_cm3 / 1_s;  // sr!
	auto Egamma = 10_GeV;
	Vector3QLength r{8.1_kpc, 0, 0};

	auto e = i->spectralEmissivity(r, Egamma) / units;

	EXPECT_NEAR(static_cast<double>(e), 1e-33, 1e-34);
}

TEST(DarkMatterIntegrator, spectralEmissivityDecay) {
	auto dmSpectrum = std::make_shared<darkmatter::PPPC4DMIDSpectrum>(
	    darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu, darkmatter::Mass::m100GeV));

	auto dmProfile = std::make_shared<darkmatter::NFWGProfile>(1., 18., 0.7 * 8e11 * sun_mass);

	const auto decayTime = 1e28_s;
	auto i = std::make_shared<DarkMatterIntegrator>(dmSpectrum, dmProfile, DarkMatterProcess::decay, 3e-26_cm3 / 1_s,
	                                                0.5, decayTime);

	auto Egamma = 10_GeV;
	Vector3QLength r{8.1_kpc, 0, 0};
	const auto restMassEnergy = dmSpectrum->getRestMassEnergy();
	const auto dNdE = dmSpectrum->getParticlesPerEnergy(Egamma);
	const auto density = dmProfile->getMassDensity(r.getR());

	const auto expected = pow<2>(c_light) / (decayTime * restMassEnergy) * dNdE * density;
	const auto got = i->spectralEmissivity(r, Egamma);

	EXPECT_NEAR(static_cast<double>(got / expected), 1., 1e-12);
}

TEST(DarkMatterIntegrator, rejectsNullInputs) {
	auto dmSpectrum = std::make_shared<darkmatter::PPPC4DMIDSpectrum>(
	    darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu, darkmatter::Mass::m100GeV));
	auto dmProfile = std::make_shared<darkmatter::NFWGProfile>(1., 18., 0.7 * 8e11 * sun_mass);

	EXPECT_THROW(DarkMatterIntegrator(nullptr, dmProfile), std::invalid_argument);
	EXPECT_THROW(DarkMatterIntegrator(dmSpectrum, nullptr), std::invalid_argument);
}

TEST(DarkMatterIntegrator, rejectsInvalidDecayParameters) {
	auto dmSpectrum = std::make_shared<darkmatter::PPPC4DMIDSpectrum>(
	    darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu, darkmatter::Mass::m100GeV));
	auto dmProfile = std::make_shared<darkmatter::NFWGProfile>(1., 18., 0.7 * 8e11 * sun_mass);

	EXPECT_THROW(DarkMatterIntegrator(dmSpectrum, dmProfile, DarkMatterProcess::decay, 3e-26_cm3 / 1_s, 0.5, 0_s),
	             std::invalid_argument);
	EXPECT_THROW(DarkMatterIntegrator(dmSpectrum, dmProfile, DarkMatterProcess::decay, 3e-26_cm3 / 1_s, 0.5, -1_s),
	             std::invalid_argument);
}

TEST(DarkMatterIntegrator, rejectsInvalidAnnihilationParameters) {
	auto dmSpectrum = std::make_shared<darkmatter::PPPC4DMIDSpectrum>(
	    darkmatter::PPPC4DMIDSpectrum(darkmatter::Channel::mu, darkmatter::Mass::m100GeV));
	auto dmProfile = std::make_shared<darkmatter::NFWGProfile>(1., 18., 0.7 * 8e11 * sun_mass);

	EXPECT_THROW(DarkMatterIntegrator(dmSpectrum, dmProfile, DarkMatterProcess::annihilation,
	                                  QAnnihilationCrossSection(0), 0.5, 1e28_s),
	             std::invalid_argument);
	EXPECT_THROW(
	    DarkMatterIntegrator(dmSpectrum, dmProfile, DarkMatterProcess::annihilation, 3e-26_cm3 / 1_s, -0.1, 1e28_s),
	    std::invalid_argument);
}

TEST(DarkMatterIntegrator, gammaAbsorptionToggleAndCoefficient) {
	auto dmSpectrum = std::make_shared<ConstantDarkMatterSpectrum>();
	auto dmProfile = std::make_shared<ConstantGalacticProfile>();
	DarkMatterIntegrator integrator(dmSpectrum, dmProfile);

	EXPECT_FALSE(integrator.isAbsorptionEnabled());
	integrator.setAbsorptionEnabled(true);
	EXPECT_TRUE(integrator.isAbsorptionEnabled());

	EXPECT_EQ(integrator.absorptionCoefficient(0_GeV), QInverseLength(0));
	EXPECT_GT(static_cast<double>(integrator.absorptionCoefficient(1_PeV)), 0.);
}

TEST(DarkMatterIntegrator, gammaAbsorptionPrecomputedTable) {
	auto dmSpectrum = std::make_shared<ConstantDarkMatterSpectrum>();
	auto dmProfile = std::make_shared<ConstantGalacticProfile>();
	DarkMatterIntegrator integrator(dmSpectrum, dmProfile, 1e28_s, true);
	std::vector<QEnergy> energies = {1_PeV, 2_PeV};

	integrator.precomputeAbsorptionCoefficients(energies);
	EXPECT_EQ(integrator.getAbsorptionCoefficientTableSize(), energies.size());

	const auto direct = integrator.absorptionCoefficient(energies[1]);
	const auto cached = integrator.getAbsorptionCoefficient(energies[1]);
	EXPECT_NEAR(static_cast<double>(cached / direct), 1., 1e-12);

	integrator.clearAbsorptionCoefficientTable();
	EXPECT_EQ(integrator.getAbsorptionCoefficientTableSize(), std::size_t(0));
}

TEST(DarkMatterIntegrator, gammaAbsorptionAttenuatesLOS) {
	auto dmSpectrum = std::make_shared<ConstantDarkMatterSpectrum>();
	auto dmProfile = std::make_shared<ConstantGalacticProfile>();
	DarkMatterIntegrator unabsorbed(dmSpectrum, dmProfile, 1e28_s, false);
	DarkMatterIntegrator absorbed(dmSpectrum, dmProfile, 1e28_s, true);

	QDirection direction = {90_deg, 0_deg};
	const auto E_gamma = 1_PeV;
	const auto unabsorbedIntensity = unabsorbed.integrateOverLOS(direction, E_gamma);
	const auto absorbedIntensity = absorbed.integrateOverLOS(direction, E_gamma);

	EXPECT_GT(static_cast<double>(unabsorbedIntensity), 0.);
	EXPECT_GE(static_cast<double>(absorbedIntensity), 0.);
	EXPECT_LT(static_cast<double>(absorbedIntensity), static_cast<double>(unabsorbedIntensity));
}

TEST(DarkMatterNeutronIntegrator, decayInverseLength) {
	auto dmSpectrum = std::make_shared<ConstantDarkMatterSpectrum>();
	auto dmProfile = std::make_shared<ConstantGalacticProfile>();
	DarkMatterNeutronIntegrator integrator(dmSpectrum, dmProfile);

	const auto energy = 1_EeV;
	const auto expected = QInverseLength(1. / (energy / (m_neutron * c_squared)) / (878.4 * 1_s) / c_light);

	EXPECT_EQ(integrator.decayInverseLength(0_GeV), QInverseLength(0));
	EXPECT_NEAR(static_cast<double>(integrator.decayInverseLength(energy) / expected), 1., 1e-12);
}

TEST(DarkMatterNeutronIntegrator, rejectsInvalidInputs) {
	auto dmSpectrum = std::make_shared<ConstantDarkMatterSpectrum>();
	auto dmProfile = std::make_shared<ConstantGalacticProfile>();

	EXPECT_THROW(DarkMatterNeutronIntegrator(nullptr, dmProfile), std::invalid_argument);
	EXPECT_THROW(DarkMatterNeutronIntegrator(dmSpectrum, nullptr), std::invalid_argument);
	EXPECT_THROW(DarkMatterNeutronIntegrator(dmSpectrum, dmProfile, DarkMatterProcess::annihilation,
	                                         QAnnihilationCrossSection(0), 0.5, 1e28_s),
	             std::invalid_argument);
	EXPECT_THROW(DarkMatterNeutronIntegrator(dmSpectrum, dmProfile, DarkMatterProcess::annihilation, 3e-26_cm3 / 1_s,
	                                         -0.1, 1e28_s),
	             std::invalid_argument);
	EXPECT_THROW(
	    DarkMatterNeutronIntegrator(dmSpectrum, dmProfile, DarkMatterProcess::decay, 3e-26_cm3 / 1_s, 0.5, 0_s),
	    std::invalid_argument);
}

TEST(DarkMatterNeutronIntegrator, decayToggle) {
	auto dmSpectrum = std::make_shared<ConstantDarkMatterSpectrum>();
	auto dmProfile = std::make_shared<ConstantGalacticProfile>();
	DarkMatterIntegrator unattenuated(dmSpectrum, dmProfile, 1e28_s, false);
	DarkMatterNeutronIntegrator neutron(dmSpectrum, dmProfile, 1e28_s);

	EXPECT_TRUE(neutron.isDecayEnabled());
	neutron.setIncludeDecay(false);
	EXPECT_FALSE(neutron.isDecayEnabled());

	QDirection direction = {90_deg, 0_deg};
	const auto energy = 1_EeV;
	const auto unattenuatedIntensity = unattenuated.integrateOverLOS(direction, energy);
	const auto neutronIntensity = neutron.integrateOverLOS(direction, energy);

	EXPECT_GT(static_cast<double>(unattenuatedIntensity), 0.);
	EXPECT_NEAR(static_cast<double>(neutronIntensity / unattenuatedIntensity), 1., 1e-10);
}

TEST(DarkMatterNeutronIntegrator, neutronDecayAttenuatesLOS) {
	auto dmSpectrum = std::make_shared<ConstantDarkMatterSpectrum>();
	auto dmProfile = std::make_shared<ConstantGalacticProfile>();
	DarkMatterIntegrator unattenuated(dmSpectrum, dmProfile, 1e28_s, false);
	DarkMatterNeutronIntegrator neutron(dmSpectrum, dmProfile, 1e28_s);

	QDirection direction = {90_deg, 0_deg};
	const auto energy = 1_EeV;
	const auto unattenuatedIntensity = unattenuated.integrateOverLOS(direction, energy);
	const auto neutronIntensity = neutron.integrateOverLOS(direction, energy);

	EXPECT_GT(static_cast<double>(unattenuatedIntensity), 0.);
	EXPECT_GE(static_cast<double>(neutronIntensity), 0.);
	EXPECT_LT(static_cast<double>(neutronIntensity), static_cast<double>(unattenuatedIntensity));
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

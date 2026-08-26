#include <atomic>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <future>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {
namespace {

class TemporarySpectrum {
  private:
	std::filesystem::path path;

  public:
	explicit TemporarySpectrum(const std::string &contents) {
		static std::atomic<unsigned long> sequence{0};
		const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
		path = std::filesystem::temp_directory_path() / ("hermes-tabulated-cr-" + std::to_string(timestamp) + "-" +
		                                                 std::to_string(sequence.fetch_add(1)) + ".txt");

		std::ofstream output(path);
		if (!output) {
			throw std::runtime_error("failed to create temporary spectrum");
		}
		output << contents;
		if (!output) {
			throw std::runtime_error("failed to write temporary spectrum");
		}
	}

	~TemporarySpectrum() {
		std::error_code error;
		std::filesystem::remove(path, error);
	}

	std::string filename() const { return path.string(); }
};

QDiffIntensity defaultIntensityUnit() { return 1. / (1_GeV * 1_cm2 * 1_s * 1_sr); }

double expectedProtonBeta(const QEnergy &kineticEnergy) {
	const double gamma = 1. + static_cast<double>(kineticEnergy / (m_proton * c_squared));
	return std::sqrt(1. - 1. / (gamma * gamma));
}

void expectNearRelative(const QPDensityPerEnergy &actual, const QPDensityPerEnergy &expected,
                        double tolerance = 1e-12) {
	ASSERT_NE(expected, QPDensityPerEnergy(0));
	EXPECT_NEAR(static_cast<double>(actual / expected), 1., tolerance);
}

}  // namespace

TEST(TabulatedCR, ReadsCommentsAndInterpolatesPowerLawInLogSpace) {
	TemporarySpectrum table(
	    "# energy [GeV]  intensity [(GeV cm2 s sr)^-1]\n"
	    "\n"
	    "1    100\n"
	    "10   1     # inline comment\n"
	    "100  0.01\n");
	cosmicrays::TabulatedCR model(table.filename(), 1_kpc, 5);

	const QEnergy energy = std::sqrt(10.) * 1_GeV;
	const QPDensityPerEnergy expected =
	    10. * defaultIntensityUnit() * (4_pi * 1_sr) / (expectedProtonBeta(energy) * c_light);
	expectNearRelative(model.getDensityPerEnergy(energy, Vector3QLength(0)), expected);

	const auto tableAxis = model.getTabulatedEnergyAxis();
	ASSERT_EQ(tableAxis.size(), 3u);
	EXPECT_EQ(tableAxis.front(), 1_GeV);
	EXPECT_EQ(tableAxis.back(), 100_GeV);

	const auto integrationAxis = model.getEnergyAxis();
	ASSERT_EQ(integrationAxis.size(), 5u);
	EXPECT_EQ(integrationAxis.front(), 1_GeV);
	EXPECT_EQ(integrationAxis.back(), 100_GeV);
	EXPECT_NEAR(model.getEnergyScaleFactor(), std::sqrt(10.), 1e-12);
	EXPECT_TRUE(model.existsScaleFactor());
	EXPECT_EQ(model.getPID(), Proton);
}

TEST(TabulatedCR, AppliesSymmetricGaussianVerticalProfile) {
	TemporarySpectrum table("1 10\n100 0.001\n");
	cosmicrays::TabulatedCR model(table.filename(), 2_kpc, 10);
	const QEnergy energy = 10_GeV;
	const QPDensityPerEnergy midplane = model.getDensityPerEnergy(energy, Vector3QLength(0));

	const auto atPositiveSigma = model.getDensityPerEnergy(energy, Vector3QLength(20_kpc, -4_kpc, 2_kpc));
	const auto atNegativeSigma = model.getDensityPerEnergy(energy, Vector3QLength(-3_kpc, 8_kpc, -2_kpc));
	const auto atTwoSigma = model.getDensityPerEnergy(energy, Vector3QLength(0_kpc, 0_kpc, 4_kpc));

	EXPECT_NEAR(static_cast<double>(atPositiveSigma / midplane), std::exp(-0.5), 1e-12);
	EXPECT_NEAR(static_cast<double>(atNegativeSigma / midplane), std::exp(-0.5), 1e-12);
	EXPECT_NEAR(static_cast<double>(atTwoSigma / midplane), std::exp(-2.), 1e-12);
	EXPECT_EQ(model.getSigmaZ(), 2_kpc);
}

TEST(TabulatedCR, SupportsExplicitColumnUnits) {
	TemporarySpectrum table("1 100\n10 1\n100 0.01\n");
	const QDiffIntensity intensityUnit = 1. / (1_TeV * 1_m2 * 1_s * 1_sr);
	cosmicrays::TabulatedCR model(table.filename(), 1_TeV, intensityUnit, 500_pc, 7);

	const QEnergy energy = std::sqrt(10.) * 1_TeV;
	const QPDensityPerEnergy expected = 10. * intensityUnit * (4_pi * 1_sr) / (expectedProtonBeta(energy) * c_light);
	expectNearRelative(model.getDensityPerEnergy(energy, Vector3QLength(0)), expected);
	EXPECT_EQ(model.getTabulatedEnergyAxis().front(), 1_TeV);
	EXPECT_EQ(model.getSigmaZ(), 500_pc);
}

TEST(TabulatedCR, ConvertsIntensityUsingEnergyDependentProtonSpeed) {
	TemporarySpectrum table("0.1 1\n10 1\n");
	cosmicrays::TabulatedCR model(table.filename());
	const QEnergy energy = 0.1_GeV;
	const QPDensityPerEnergy ultraRelativisticApproximation = defaultIntensityUnit() * (4_pi * 1_sr) / c_light;
	const QPDensityPerEnergy expected = ultraRelativisticApproximation / expectedProtonBeta(energy);
	const QPDensityPerEnergy actual = model.getDensityPerEnergy(energy, Vector3QLength(0));

	expectNearRelative(actual, expected);
	EXPECT_GT(actual, ultraRelativisticApproximation);
}

TEST(TabulatedCR, ReturnsZeroOutsideTabulatedEnergyRange) {
	TemporarySpectrum table("1 10\n10 0.1\n");
	cosmicrays::TabulatedCR model(table.filename());
	const Vector3QLength position(0);

	EXPECT_EQ(model.getDensityPerEnergy(0.99_GeV, position), QPDensityPerEnergy(0));
	EXPECT_EQ(model.getDensityPerEnergy(10.01_GeV, position), QPDensityPerEnergy(0));
	EXPECT_EQ(model.getDensityPerEnergy(QEnergy(std::numeric_limits<double>::quiet_NaN()), position),
	          QPDensityPerEnergy(0));
	EXPECT_GT(static_cast<double>(model.getDensityPerEnergy(1_GeV, position)), 0.);
	EXPECT_GT(static_cast<double>(model.getDensityPerEnergy(10_GeV, position)), 0.);
}

TEST(TabulatedCR, FeedsPiZeroEnergyIntegration) {
	const auto gasData = getDataPath("GasDensity/Remy18/NHrings_Ts300K.fits.gz");
	if (!std::filesystem::is_regular_file(gasData)) {
		GTEST_SKIP() << "HERMES gas data are not available";
	}

	TemporarySpectrum table("1e3 1e-8\n1e5 1e-12\n1e8 1e-18\n");
	auto model = std::make_shared<cosmicrays::TabulatedCR>(table.filename(), 1_kpc, 100);
	auto gas = std::make_shared<neutralgas::RingModel>(neutralgas::GasType::HI);
	auto crossSection = std::make_shared<interactions::Kamae06Gamma>();
	PiZeroIntegrator integrator(model, gas, crossSection);

	const auto inPlane = integrator.integrateOverEnergy(Vector3QLength(8.5_kpc, 0_kpc, 0_kpc), 1_TeV);
	const auto atOneSigma = integrator.integrateOverEnergy(Vector3QLength(8.5_kpc, 0_kpc, 1_kpc), 1_TeV);

	EXPECT_TRUE(std::isfinite(static_cast<double>(inPlane)));
	EXPECT_GT(static_cast<double>(inPlane), 0.);
	EXPECT_NEAR(static_cast<double>(atOneSigma / inPlane), std::exp(-0.5), 1e-12);
}

TEST(TabulatedCR, RejectsInvalidConfigurationAndTables) {
	const std::string missing =
	    (std::filesystem::temp_directory_path() / "hermes-tabulated-cr-file-that-does-not-exist.txt").string();
	EXPECT_THROW(static_cast<void>(cosmicrays::TabulatedCR(missing)), std::runtime_error);

	TemporarySpectrum valid("1 10\n10 1\n");
	EXPECT_THROW(cosmicrays::TabulatedCR(valid.filename(), 0_kpc, 10), std::invalid_argument);
	EXPECT_THROW(cosmicrays::TabulatedCR(valid.filename(), QLength(std::numeric_limits<double>::quiet_NaN()), 10),
	             std::invalid_argument);
	EXPECT_THROW(cosmicrays::TabulatedCR(valid.filename(), 1_kpc, 1), std::invalid_argument);
	EXPECT_THROW(cosmicrays::TabulatedCR(valid.filename(), QEnergy(0), defaultIntensityUnit(), 1_kpc, 10),
	             std::invalid_argument);
	EXPECT_THROW(cosmicrays::TabulatedCR(valid.filename(), 1_GeV, QDiffIntensity(0), 1_kpc, 10), std::invalid_argument);

	TemporarySpectrum tooShort("1 10\n");
	EXPECT_THROW(static_cast<void>(cosmicrays::TabulatedCR(tooShort.filename())), std::invalid_argument);

	TemporarySpectrum unsorted("10 1\n1 10\n");
	EXPECT_THROW(static_cast<void>(cosmicrays::TabulatedCR(unsorted.filename())), std::invalid_argument);

	TemporarySpectrum duplicate("1 10\n1 9\n");
	EXPECT_THROW(static_cast<void>(cosmicrays::TabulatedCR(duplicate.filename())), std::invalid_argument);

	TemporarySpectrum nonPositive("1 10\n10 0\n");
	EXPECT_THROW(static_cast<void>(cosmicrays::TabulatedCR(nonPositive.filename())), std::invalid_argument);

	TemporarySpectrum malformed("1 10\n10 1 unexpected\n");
	EXPECT_THROW(static_cast<void>(cosmicrays::TabulatedCR(malformed.filename())), std::invalid_argument);
}

TEST(TabulatedCR, ConcurrentReadsAreDeterministic) {
	TemporarySpectrum table("1 100\n10 1\n100 0.01\n");
	const cosmicrays::TabulatedCR model(table.filename(), 1_kpc, 40);
	const QEnergy energy = 7_GeV;
	const Vector3QLength position(3_kpc, 2_kpc, 0.4_kpc);
	const QPDensityPerEnergy expected = model.getDensityPerEnergy(energy, position);

	std::vector<std::future<QPDensityPerEnergy>> reads;
	for (int worker = 0; worker < 8; ++worker) {
		reads.push_back(std::async(std::launch::async, [&model, energy, position] {
			QPDensityPerEnergy result(0);
			for (int iteration = 0; iteration < 1000; ++iteration) {
				result = model.getDensityPerEnergy(energy, position);
			}
			return result;
		}));
	}

	for (auto &read : reads) {
		EXPECT_EQ(read.get(), expected);
	}
}

TEST(TabulatedCR, ConcurrentPiZeroEnergyIntegrationsAreDeterministic) {
	const auto gasData = getDataPath("GasDensity/Remy18/NHrings_Ts300K.fits.gz");
	const auto crossSectionData = getDataPath("Interactions/AAfrag2021Gamma.txt.gz");
	if (!std::filesystem::is_regular_file(gasData) || !std::filesystem::is_regular_file(crossSectionData)) {
		GTEST_SKIP() << "HERMES gas or interaction data are not available";
	}

	TemporarySpectrum table("1e2 1e-4\n1e4 1e-8\n1e6 1e-12\n1e8 1e-16\n");
	auto model = std::make_shared<cosmicrays::TabulatedCR>(table.filename(), 1_kpc, 100);
	auto gas = std::make_shared<neutralgas::RingModel>(neutralgas::GasType::H2);
	auto crossSection = std::make_shared<interactions::AAfragGamma>();
	const PiZeroAbsorptionIntegrator integrator(model, gas, crossSection);
	const QEnergy energy = 10_TeV;
	const Vector3QLength position(4_kpc, 0_kpc, 0.2_kpc);
	const QPiZeroIntegral expected = integrator.integrateOverEnergy(position, energy);

	std::vector<std::future<QPiZeroIntegral>> integrations;
	for (int worker = 0; worker < 8; ++worker) {
		integrations.push_back(std::async(std::launch::async, [&integrator, position, energy] {
			QPiZeroIntegral result(0);
			for (int iteration = 0; iteration < 250; ++iteration) {
				result = integrator.integrateOverEnergy(position, energy);
			}
			return result;
		}));
	}

	for (auto &integration : integrations) {
		EXPECT_EQ(integration.get(), expected);
	}
}

}  // namespace hermes

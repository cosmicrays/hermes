#include <chrono>
#include <cmath>
#include <memory>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

class TestCRDensity : public cosmicrays::CosmicRayDensity {
  private:
	QEnergy E_c;
	QFrequency freq;

  public:
	TestCRDensity(QFrequency freq_) {
		freq = freq_;
		E_c = sqrt(m_electron * freq * 4_pi / (3 * e_plus * 1_muG)) *
		      m_electron * c_squared;

		// add some energies over which the integrator integrates
		// including E(freq_c) for which dn/dE contributes only
		// deltaE = 1 eV
		energyRange.push_back(E_c - 2_eV);
		energyRange.push_back(E_c - 1_eV);
		energyRange.push_back(E_c);
		energyRange.push_back(E_c + 1_eV);
		energyRange.push_back(E_c + 2_eV);
	}
	QEnergy getElectronEnergy() { return E_c; }
	// dn/dE is delta-function reproduce F(1) \approx 0.655
	QPDensityPerEnergy getDensityPerEnergy(const QEnergy &E_,
	                                       const Vector3QLength &pos_) const {
		if (E_ == E_c) {
			return 1.0 / (1_m3 * 1_J);
		} else {
			return 0;
		}
	}
};

class ConstantEmissivityPiZero : public PiZeroIntegrator {
  private:
	QPiZeroIntegral emissivity;

  public:
	ConstantEmissivityPiZero(const std::shared_ptr<neutralgas::RingModel> &ringModel,
	                         const QPiZeroIntegral &emissivity_)
	    : PiZeroIntegrator(std::make_shared<cosmicrays::DummyCR>(), ringModel,
	                       std::make_shared<interactions::DummyCrossSection>(QDiffCrossSection(0))),
	      emissivity(emissivity_) {}

	QPiZeroIntegral integrateOverEnergy(const Vector3QLength &, const QEnergy &) const override { return emissivity; }

	std::vector<LOSSegment> ringIntervals(const neutralgas::Ring &ring, const QDirection &direction) const {
		return getRingLOSIntervals(ring, direction);
	}
};

class ConstantGasProfile : public neutralgas::ProfileAbstract {
  public:
	QPDensity getPDensity(neutralgas::GasType, const Vector3QLength &) const override { return 1.0 / 1_cm3; }
};

class ConstantEmissivityAbsorption : public PiZeroAbsorptionIntegrator {
  private:
	QPiZeroIntegral emissivity;

  public:
	ConstantEmissivityAbsorption(const std::shared_ptr<neutralgas::RingModel> &ringModel,
	                             const QPiZeroIntegral &emissivity_)
	    : PiZeroAbsorptionIntegrator(std::make_shared<cosmicrays::DummyCR>(), ringModel,
	                                 std::make_shared<interactions::DummyCrossSection>(QDiffCrossSection(0))),
	      emissivity(emissivity_) {
		dProfile = std::make_shared<ConstantGasProfile>();
	}

	QPiZeroIntegral integrateOverEnergy(const Vector3QLength &, const QEnergy &) const override { return emissivity; }

	std::vector<LOSSegment> ringIntervals(const neutralgas::Ring &ring, const QDirection &direction) const {
		return getRingLOSIntervals(ring, direction);
	}
};

class ConstantEmissivityNeutronDecay : public PiZeroNeutronDecayIntegrator {
  private:
	QPiZeroIntegral emissivity;

  public:
	ConstantEmissivityNeutronDecay(const std::shared_ptr<neutralgas::RingModel> &ringModel,
	                               const QPiZeroIntegral &emissivity_)
	    : PiZeroNeutronDecayIntegrator(std::make_shared<cosmicrays::DummyCR>(), ringModel,
	                                   std::make_shared<interactions::DummyCrossSection>(QDiffCrossSection(0))),
	      emissivity(emissivity_) {
		dProfile = std::make_shared<ConstantGasProfile>();
	}

	QPiZeroIntegral integrateOverEnergy(const Vector3QLength &, const QEnergy &) const override { return emissivity; }

	std::vector<LOSSegment> ringIntervals(const neutralgas::Ring &ring, const QDirection &direction) const {
		return getRingLOSIntervals(ring, direction);
	}
};

template <typename Intervals>
std::pair<QLength, QLength> geometricAndAttenuatedLengths(
    const Intervals &intervals, const QInverseLength &inverseLength) {
	QLength geometricLength(0);
	QLength attenuatedLength(0);
	for (const auto &interval : intervals) {
		geometricLength += interval.second - interval.first;
		attenuatedLength +=
		    (exp(-inverseLength * interval.first) - exp(-inverseLength * interval.second)) / inverseLength;
	}
	return {geometricLength, attenuatedLength};
}

TEST(PiZeroIntegrator, LOSIntegrationStepsAreConfigurable) {
	auto ringModel = std::make_shared<neutralgas::RingModel>(neutralgas::GasType::HI);
	ConstantEmissivityPiZero integrator(ringModel, QPiZeroIntegral(1));

	EXPECT_EQ(integrator.getLOSIntegrationSteps(), PiZeroIntegrator::DefaultLOSIntegrationSteps);
	integrator.setLOSIntegrationSteps(128);
	EXPECT_EQ(integrator.getLOSIntegrationSteps(), 128u);
	EXPECT_THROW(integrator.setLOSIntegrationSteps(1), std::invalid_argument);
}

TEST(PiZeroIntegrator, RingLOSIntervalsMatchAnalyticCrossings) {
	auto ringModel = std::make_shared<neutralgas::RingModel>(neutralgas::GasType::H2);
	ConstantEmissivityPiZero integrator(ringModel, QPiZeroIntegral(1));
	const auto ring = (*ringModel)[3];  // cylindrical annulus 3--4 kpc

	const auto centreIntervals = integrator.ringIntervals(*ring, QDirection({90_deg, 0_deg}));
	ASSERT_EQ(centreIntervals.size(), 2u);
	EXPECT_NEAR(static_cast<double>(centreIntervals[0].first / 1_kpc), 4.5, 1e-12);
	EXPECT_NEAR(static_cast<double>(centreIntervals[0].second / 1_kpc), 5.5, 1e-12);
	EXPECT_NEAR(static_cast<double>(centreIntervals[1].first / 1_kpc), 11.5, 1e-12);
	EXPECT_NEAR(static_cast<double>(centreIntervals[1].second / 1_kpc), 12.5, 1e-12);

	const QAngle tangentLongitude = std::asin(4.0 / 8.5) * 1_rad;
	const auto tangentIntervals = integrator.ringIntervals(*ring, QDirection({90_deg, tangentLongitude}));
	EXPECT_TRUE(tangentIntervals.empty());

	const double impactParameter = 3.99;
	const QAngle nearTangentLongitude = std::asin(impactParameter / 8.5) * 1_rad;
	const QAngle latitude = 5_deg;
	const double projectedRayLength = std::cos(static_cast<double>(latitude / 1_rad));
	const double intervalCentre =
	    8.5 * std::cos(static_cast<double>(nearTangentLongitude / 1_rad)) / projectedRayLength;
	const double intervalHalfWidth = std::sqrt(4.0 * 4.0 - impactParameter * impactParameter) / projectedRayLength;
	const auto nearTangentIntervals =
	    integrator.ringIntervals(*ring, QDirection({90_deg - latitude, nearTangentLongitude}));
	ASSERT_EQ(nearTangentIntervals.size(), 1u);
	EXPECT_NEAR(static_cast<double>(nearTangentIntervals[0].first / 1_kpc), intervalCentre - intervalHalfWidth, 1e-11);
	EXPECT_NEAR(static_cast<double>(nearTangentIntervals[0].second / 1_kpc), intervalCentre + intervalHalfWidth, 1e-11);
}

TEST(PiZeroIntegrator, ConstantEmissivityExactlyCancelsRingNormalization) {
	auto ringModel = std::make_shared<neutralgas::RingModel>(neutralgas::GasType::H2);
	std::array<bool, 12> enabledRings{};
	enabledRings[5] = true;  // narrow crossing around l=44.9 degrees
	ringModel->setEnabledRings(enabledRings);

	const QPiZeroIntegral emissivity(2.5);
	ConstantEmissivityPiZero integrator(ringModel, emissivity);
	integrator.setLOSIntegrationSteps(10);
	const QDirection direction = {90_deg, 44.9_deg};
	const QColumnDensity columnDensity = (*ringModel)[5]->getColumnDensity(direction);
	ASSERT_GT(static_cast<double>(columnDensity), 0.0);

	const QDiffIntensity expected = columnDensity * emissivity / (4_pi * 1_sr);
	const QDiffIntensity actual = integrator.integrateOverLOS(direction, 10_TeV);
	EXPECT_NEAR(static_cast<double>(actual / expected), 1.0, 1e-12);
}

TEST(PiZeroAbsorptionIntegrator, MatchesAnalyticExponentialAttenuation) {
	auto ringModel = std::make_shared<neutralgas::RingModel>(neutralgas::GasType::H2);
	std::array<bool, 12> enabledRings{};
	enabledRings[3] = true;
	ringModel->setEnabledRings(enabledRings);

	const QPiZeroIntegral emissivity(2.5);
	ConstantEmissivityAbsorption integrator(ringModel, emissivity);
	const QDirection direction = {90_deg, 15_deg};
	const QEnergy energy = 1_PeV;
	const QInverseLength inverseLength = integrator.absorptionCoefficient(energy);
	ASSERT_GT(static_cast<double>(inverseLength), 0.0);
	EXPECT_THROW(integrator.absorptionCoefficient(QEnergy(0)), std::invalid_argument);

	const auto intervals = integrator.ringIntervals(*(*ringModel)[3], direction);
	ASSERT_EQ(intervals.size(), 2u);
	const auto lengths = geometricAndAttenuatedLengths(intervals, inverseLength);
	const QColumnDensity columnDensity = (*ringModel)[3]->getColumnDensity(direction);
	ASSERT_GT(static_cast<double>(columnDensity), 0.0);

	const QDiffIntensity unabsorbed = columnDensity * emissivity / (4_pi * 1_sr);
	const QDiffIntensity expected = unabsorbed * lengths.second / lengths.first;
	const QDiffIntensity actual = integrator.integrateOverLOS(direction, energy);
	EXPECT_NEAR(static_cast<double>(actual / expected), 1.0, 1e-11);
	EXPECT_LT(actual, unabsorbed);
}

TEST(PiZeroNeutronDecayIntegrator, MatchesAnalyticExponentialSurvival) {
	auto ringModel = std::make_shared<neutralgas::RingModel>(neutralgas::GasType::H2);
	std::array<bool, 12> enabledRings{};
	enabledRings[3] = true;
	ringModel->setEnabledRings(enabledRings);

	const QPiZeroIntegral emissivity(2.5);
	ConstantEmissivityNeutronDecay integrator(ringModel, emissivity);
	const QDirection direction = {90_deg, 15_deg};
	const QEnergy energy = 1_EeV;
	const QInverseLength inverseLength = integrator.decayInverseLength(energy);
	ASSERT_GT(static_cast<double>(inverseLength), 0.0);
	EXPECT_THROW(integrator.decayInverseLength(QEnergy(0)), std::invalid_argument);

	const auto intervals = integrator.ringIntervals(*(*ringModel)[3], direction);
	ASSERT_EQ(intervals.size(), 2u);
	const auto lengths = geometricAndAttenuatedLengths(intervals, inverseLength);
	const QColumnDensity columnDensity = (*ringModel)[3]->getColumnDensity(direction);
	ASSERT_GT(static_cast<double>(columnDensity), 0.0);

	const QDiffIntensity undecayed = columnDensity * emissivity / (4_pi * 1_sr);
	const QDiffIntensity expected = undecayed * lengths.second / lengths.first;
	const QDiffIntensity actual = integrator.integrateOverLOS(direction, energy);
	EXPECT_NEAR(static_cast<double>(actual / expected), 1.0, 1e-11);
	EXPECT_LT(actual, undecayed);

	integrator.setLOSIntegrationSteps(500);
	const QDiffIntensity pevResult500 = integrator.integrateOverLOS(direction, 1_PeV);
	integrator.setLOSIntegrationSteps(1000);
	const QDiffIntensity pevResult1000 = integrator.integrateOverLOS(direction, 1_PeV);
	EXPECT_LT(std::fabs(static_cast<double>(pevResult500 / undecayed)), 1e-100);
	EXPECT_LT(std::fabs(static_cast<double>((pevResult500 - pevResult1000) / undecayed)), 1e-100);
}

TEST(PiZeroIntegrator, integrateOverEnergy) {
	auto cr_proton = std::make_shared<cosmicrays::SimpleCR>(
	    cosmicrays::SimpleCR());
	// std::vector<PID> particletypes = {Proton};
	// auto cr_proton = std::make_shared<cosmicrays::Dragon2D>(
	//    cosmicrays::Dragon2D(particletypes));

	// interaction
	auto kamae = std::make_shared<interactions::Kamae06Gamma>(
	    interactions::Kamae06Gamma());
	// HI model
	auto ringModel = std::make_shared<neutralgas::RingModel>(
	    neutralgas::RingModel(neutralgas::GasType::HI));
	// integrator
	auto intPiZero = std::make_shared<PiZeroIntegrator>(
	    PiZeroIntegrator(cr_proton, ringModel, kamae));

	auto res =
	    intPiZero->integrateOverEnergy(Vector3QLength(1_kpc, 0, 0), 10_TeV);

	EXPECT_NEAR(static_cast<double>(res), 5.7647e-33, 1e-35);

	// test cache
	intPiZero->setupCacheTable(50, 50, 10);
	intPiZero->initCacheTable();
	auto res_cache =
	    intPiZero->integrateOverEnergy(Vector3QLength(1_kpc, 0, 0), 10_TeV);

	// EXPECT_GT(static_cast<double>(res_cache), 0);
	// EXPECT_NEAR(static_cast<double>(res), static_cast<double>(res_cache),
	// 1e-36);
}

TEST(PiZeroIntegrator, ChannelsRatio) {
	auto cr_proton = std::make_shared<cosmicrays::SimpleCR>(
	    cosmicrays::SimpleCR(Proton));
	auto cr_helium = std::make_shared<cosmicrays::SimpleCR>(
	    cosmicrays::SimpleCR(Helium));
	std::vector<std::shared_ptr<cosmicrays::CosmicRayDensity>> cr_all = {
	    cr_proton, cr_helium};

	// interaction
	auto kamae = std::make_shared<interactions::Kamae06Gamma>(
	    interactions::Kamae06Gamma());
	// HI model
	auto ringModel = std::make_shared<neutralgas::RingModel>(
	    neutralgas::RingModel(neutralgas::GasType::HI));
	// integrators
	auto intPiZero_proton = std::make_shared<PiZeroIntegrator>(
	    PiZeroIntegrator(cr_proton, ringModel, kamae));
	auto intPiZero_total = std::make_shared<PiZeroIntegrator>(
	    PiZeroIntegrator(cr_all, ringModel, kamae));

	Vector3QLength pos(8.5_kpc, 0, 0);
	QEnergy Egamma = 10_GeV;
	auto res_proton = intPiZero_proton->integrateOverEnergy(pos, Egamma);
	auto res_total = intPiZero_total->integrateOverEnergy(pos, Egamma);

	double f_He = 0.1;
	double ratio =
	    ((1.0 + f_He * 3.81) + (3.68 + f_He * 14.2)) / (1.0 + f_He * 3.81);
	EXPECT_NEAR(static_cast<double>(res_proton) * ratio,
	            static_cast<double>(res_total), 1e-22);
}

TEST(PiZeroIntegrator, PiZeroLOS) {
	// auto crdensity =
	// std::make_shared<TestCRDensity>(TestCRDensity(1_MHz));
	auto simpleModel = std::make_shared<cosmicrays::SimpleCR>(
	    cosmicrays::SimpleCR());
	auto dragonModel =
	    std::make_shared<cosmicrays::Dragon2D>(cosmicrays::Dragon2D(Proton));
	// interaction
	auto kamae = std::make_shared<interactions::Kamae06Gamma>(
	    interactions::Kamae06Gamma());
	// HI model
	auto ringModel = std::make_shared<neutralgas::RingModel>(
	    neutralgas::RingModel(neutralgas::GasType::HI));
	// integrator
	auto intPiZero = std::make_shared<PiZeroIntegrator>(
	    PiZeroIntegrator(dragonModel, ringModel, kamae));

	// skymap
	int nside = 4;
	auto skymap = std::make_shared<GammaSkymap>(GammaSkymap(nside, 1_GeV));
	skymap->setIntegrator(intPiZero);

	// auto output =
	// std::make_shared<HEALPix>(HEALPix("!test-pion.fits.gz"));

	auto pos = Vector3QLength(8.5_kpc, 0, 0);
	QDirection dir = {90_deg, 1_deg};
	// std::cerr << intPiZero->integrateOverEnergy(pos, 1_GeV) << std::endl;
	// std::cerr << intPiZero->integrateOverEnergy(pos, 100_MeV) <<
	// std::endl; std::cerr << intPiZero->integrateOverLOS(dir, 1000_MeV) <<
	// std::endl;
	skymap->compute();
	// skymap->save(output);

	// sqrt(3)*e_charge^3/(8*pi^2*epsilon_0*c*electron_mass)*0.655*1*microGauss*1/(m^3*J)*1_eV
	// EXPECT_NEAR(emissivity.getValue(), 3.915573e-55, 2e-56); // J/m^3
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

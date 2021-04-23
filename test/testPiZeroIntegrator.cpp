#include <chrono>
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

#include <chrono>
#include <iostream>
#include <memory>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

typedef CacheStorageWith2Args<QLength, QLength, QArea> CacheStorageTest;

TEST(CacheTools, getValue) {
	auto cache = std::make_shared<CacheStorageTest>(CacheStorageTest());
	auto f_area = [](const QLength &a, const QLength &b) { return a * b; };

	cache->setFunction(f_area);

	QLength a = 10_m, b = 50_m;
	EXPECT_DOUBLE_EQ(static_cast<double>(cache->getValue(a, b)),
	                 static_cast<double>(cache->getValue(a, b)));
}

TEST(CacheTools, Kamae06Gamma) {
	auto cache =
	    std::make_unique<CacheStorageCrossSection>(CacheStorageCrossSection());
	auto f_kn = std::make_shared<interactions::Kamae06Gamma>(
	    interactions::Kamae06Gamma());
	f_kn->setCachingStorage(std::move(cache));

	QDiffCrossSection integral1(0), integral2(0);
	QEnergy E_proton = 1_TeV;

	std::chrono::time_point<std::chrono::system_clock> start =
	    std::chrono::system_clock::now();
	for (int i = 0; i < 40; ++i) {
		for (QEnergy E_gamma = 1_MeV; E_gamma < 1_TeV;
		     E_gamma = E_gamma * 1.05) {
			integral1 += f_kn->getDiffCrossSection(E_proton, E_gamma) * i;
		}
	}
	std::chrono::time_point<std::chrono::system_clock> stop =
	    std::chrono::system_clock::now();
	auto milliseconds1 =
	    std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	start = std::chrono::system_clock::now();
	for (int i = 0; i < 40; ++i) {
		for (QEnergy E_gamma = 1_MeV; E_gamma < 1_TeV;
		     E_gamma = E_gamma * 1.05) {
			integral2 +=
			    f_kn->getDiffCrossSectionDirectly(E_proton, E_gamma) * i;
		}
	}
	stop = std::chrono::system_clock::now();
	auto milliseconds2 =
	    std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	EXPECT_DOUBLE_EQ(static_cast<double>(integral1),
	                 static_cast<double>(integral2));
	EXPECT_GT(milliseconds2.count(), milliseconds1.count());
}

TEST(CacheTools, BremsstrahlungSimple) {
	auto f_brem = std::make_shared<interactions::BremsstrahlungSimple>(
	    interactions::BremsstrahlungSimple());
	f_brem->enableCaching();

	QDiffCrossSection integral1(0), integral2(0);
	QEnergy E_proton = 10_GeV;
	QEnergy E_gamma = 1_GeV;
	auto t = interactions::BremsstrahlungSimple::Target::HI;

	std::chrono::time_point<std::chrono::system_clock> start =
	    std::chrono::system_clock::now();
	for (std::size_t i = 0; i < 10000; ++i) {
		integral1 += f_brem->getDiffCrossSectionForTarget(
		    t, E_gamma * (static_cast<double>(i % 100)), E_gamma);
	}
	std::chrono::time_point<std::chrono::system_clock> stop =
	    std::chrono::system_clock::now();
	auto milliseconds1 =
	    std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	f_brem->disableCaching();
	start = std::chrono::system_clock::now();
	for (std::size_t i = 0; i < 10000; ++i) {
		integral2 += f_brem->getDiffCrossSectionForTarget(
		    t, E_gamma * (static_cast<double>(i % 100)), E_gamma);
	}
	stop = std::chrono::system_clock::now();
	auto milliseconds2 =
	    std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	EXPECT_DOUBLE_EQ(static_cast<double>(integral1),
	                 static_cast<double>(integral2));
	EXPECT_GT(milliseconds2.count(), milliseconds1.count());
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

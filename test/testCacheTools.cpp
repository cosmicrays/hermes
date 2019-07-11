#include "gtest/gtest.h"
#include <memory>
#include <iostream>
#include <chrono>

#include "hermes.h"

namespace hermes {

typedef CacheStorageWith2Args<QLength,QLength,QArea> CacheStorageTest;

TEST(CacheTools, getValue) {
	auto cache = std::make_shared<CacheStorageTest>(CacheStorageTest());
	auto f_area = [](const QLength &a, const QLength &b) {return a*b;};
	
	cache->setFunction(f_area);

	QLength a = 10_m, b = 50_m;
	std::cerr << cache->getValue(a, b) << std::endl;
	std::cerr << cache->getValue(a, b) << std::endl;
}

TEST(CacheTools, KleinNishina) {
	auto cache = std::make_unique<CacheStorageKN>(CacheStorageKN());
	auto f_kn = std::make_shared<KleinNishina>(KleinNishina());
	f_kn->setCacheStorage(std::move(cache));

	QDiffCrossSection integral(0);
	QEnergy E_electron = 100_TeV;
	QEnergy E_photon = 6.626e-4_eV;
	QEnergy E_gamma = 1_TeV;
   	
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
        for (std::size_t i = 0; i < 1000000; ++i) {
		integral += f_kn->getDiffCrossSectionFromCache(E_electron, E_photon*(i%2), E_gamma);
	}
	std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cerr << "cache: " << milliseconds.count() << " ms" << std::endl;

	start = std::chrono::system_clock::now();
        for (std::size_t i = 0; i < 1000000; ++i) {
		integral += f_kn->getDiffCrossSection(E_electron, E_photon*(i%2), E_gamma);
	}
    	stop = std::chrono::system_clock::now();
	milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cerr << "direct: " << milliseconds.count() << " ms" << std::endl;
}

TEST(CacheTools, Kamae06) {
	auto cache = std::make_unique<CacheStorageKamae>(CacheStorageKamae());
	auto f_kn = std::make_shared<Kamae06>(Kamae06());
	f_kn->setCacheStorage(std::move(cache));

	QDiffCrossSection integral(0);
	QEnergy E_proton = 100_TeV;
	QEnergy E_gamma = 1_TeV;
   	
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
        for (std::size_t i = 0; i < 1000000; ++i) {
		integral += f_kn->getDiffCrossSection(E_proton*(i%2), E_gamma);
	}
	std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cerr << "cache: " << milliseconds.count() << " ms" << std::endl;

	start = std::chrono::system_clock::now();
        for (std::size_t i = 0; i < 1000000; ++i) {
		integral += f_kn->getDiffCrossSectionDirectly(E_proton*(i%2), E_gamma);
	}
    	stop = std::chrono::system_clock::now();
	milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cerr << "direct: " << milliseconds.count() << " ms" << std::endl;
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes


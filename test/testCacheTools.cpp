#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

namespace {

bool strictCacheTimingEnabled() {
	const char *env = std::getenv("HERMES_RUN_CACHE_PERF_TESTS");
	if (!env) return false;
	return std::strcmp(env, "1") == 0 || std::strcmp(env, "true") == 0 || std::strcmp(env, "on") == 0;
}

struct CacheBenchmarkResult {
	QDiffCrossSection integral_cached;
	QDiffCrossSection integral_noncached;
	std::chrono::milliseconds milliseconds_cached;
	std::chrono::milliseconds milliseconds_noncached;
};

CacheBenchmarkResult runKamae06GammaBenchmark() {
	auto cache = std::make_unique<CacheStorageCrossSection>(CacheStorageCrossSection());
	auto f_kn = std::make_shared<interactions::Kamae06Gamma>(interactions::Kamae06Gamma());
	f_kn->setCachingStorage(std::move(cache));

	CacheBenchmarkResult result{QDiffCrossSection(0), QDiffCrossSection(0), std::chrono::milliseconds(0),
	                            std::chrono::milliseconds(0)};
	QEnergy E_proton = 1_TeV;

	auto start = std::chrono::system_clock::now();
	for (int i = 0; i < 100; ++i) {
		for (QEnergy E_gamma = 1_MeV; E_gamma < 1_TeV; E_gamma = E_gamma * 1.05) {
			result.integral_cached += f_kn->getDiffCrossSection(E_proton, E_gamma) * i;
		}
	}
	auto stop = std::chrono::system_clock::now();
	result.milliseconds_cached = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	start = std::chrono::system_clock::now();
	for (int i = 0; i < 100; ++i) {
		for (QEnergy E_gamma = 1_MeV; E_gamma < 1_TeV; E_gamma = E_gamma * 1.05) {
			result.integral_noncached += f_kn->getDiffCrossSectionDirectly(E_proton, E_gamma) * i;
		}
	}
	stop = std::chrono::system_clock::now();
	result.milliseconds_noncached = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	return result;
}

CacheBenchmarkResult runBremsstrahlungGALPROPBenchmark() {
	auto f_brem = std::make_shared<interactions::BremsstrahlungGALPROP>(interactions::BremsstrahlungGALPROP());

	CacheBenchmarkResult result{QDiffCrossSection(0), QDiffCrossSection(0), std::chrono::milliseconds(0),
	                            std::chrono::milliseconds(0)};
	QEnergy E_gamma = 1_GeV;
	auto t = interactions::BremsstrahlungAbstract::Target::HI;

	f_brem->enableCaching();
	auto start = std::chrono::system_clock::now();
	for (std::size_t i = 0; i < 10000; ++i) {
		result.integral_cached +=
		    f_brem->getDiffCrossSectionForTarget(t, E_gamma * (static_cast<double>(i % 100)), E_gamma);
	}
	auto stop = std::chrono::system_clock::now();
	result.milliseconds_cached = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	f_brem->disableCaching();
	start = std::chrono::system_clock::now();
	for (std::size_t i = 0; i < 10000; ++i) {
		result.integral_noncached +=
		    f_brem->getDiffCrossSectionForTarget(t, E_gamma * (static_cast<double>(i % 100)), E_gamma);
	}
	stop = std::chrono::system_clock::now();
	result.milliseconds_noncached = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	return result;
}

}  // namespace

typedef CacheStorageWith2Args<QLength, QLength, QArea> CacheStorageTest;

TEST(CacheTools, getValue) {
	auto cache = std::make_shared<CacheStorageTest>(CacheStorageTest());
	auto f_area = [](const QLength &a, const QLength &b) { return a * b; };

	cache->setFunction(f_area);

	QLength a = 10_m, b = 50_m;
	EXPECT_DOUBLE_EQ(static_cast<double>(cache->getValue(a, b)), static_cast<double>(cache->getValue(a, b)));
}

TEST(CacheTools, Kamae06GammaCorrectness) {
	auto result = runKamae06GammaBenchmark();
	EXPECT_DOUBLE_EQ(static_cast<double>(result.integral_cached), static_cast<double>(result.integral_noncached));
}

TEST(CacheTools, Kamae06GammaPerformance) {
	if (!strictCacheTimingEnabled()) {
		GTEST_SKIP() << "Set HERMES_RUN_CACHE_PERF_TESTS=1 to enable strict cache timing assertions.";
	}
	auto result = runKamae06GammaBenchmark();
	EXPECT_GT(result.milliseconds_noncached.count(), result.milliseconds_cached.count());
}

TEST(CacheTools, BremsstrahlungGALPROPCorrectness) {
	auto result = runBremsstrahlungGALPROPBenchmark();
	EXPECT_DOUBLE_EQ(static_cast<double>(result.integral_cached), static_cast<double>(result.integral_noncached));
}

TEST(CacheTools, BremsstrahlungGALPROPPerformance) {
	if (!strictCacheTimingEnabled()) {
		GTEST_SKIP() << "Set HERMES_RUN_CACHE_PERF_TESTS=1 to enable strict cache timing assertions.";
	}
	auto result = runBremsstrahlungGALPROPBenchmark();
	EXPECT_GT(result.milliseconds_noncached.count(), result.milliseconds_cached.count());
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

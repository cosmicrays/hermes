#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

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
	EXPECT_DOUBLE_EQ(static_cast<double>(500_m2), static_cast<double>(cache->getValue(a, b)));
	EXPECT_DOUBLE_EQ(static_cast<double>(500_m2), static_cast<double>(cache->getValue(a, b)));
}

TEST(CacheTools, ConcurrentTwoArgumentAccessIsConsistent) {
	CacheStorageWith2Args<double, double, double> cache;
	cache.setFunction([](double a, double b) { return a * a + 3.0 * b; });

	std::atomic<bool> failed(false);
	std::vector<std::thread> workers;
	for (int thread = 0; thread < 8; ++thread) {
		workers.emplace_back([&cache, &failed, thread]() {
			for (int iteration = 0; iteration < 2000; ++iteration) {
				const double a = static_cast<double>((iteration + thread) % 37);
				const double b = static_cast<double>((iteration * 3 + thread) % 29);
				if (cache.getValue(a, b) != a * a + 3.0 * b) failed = true;
			}
		});
	}
	for (auto &worker : workers) worker.join();
	EXPECT_FALSE(failed.load());
}

TEST(CacheTools, ConcurrentThreeArgumentAccessIsConsistent) {
	CacheStorageWith3Args<double, double, double, double> cache;
	cache.setFunction([](double a, double b, double c) { return a + 2.0 * b - c; });

	std::atomic<bool> failed(false);
	std::vector<std::thread> workers;
	for (int thread = 0; thread < 8; ++thread) {
		workers.emplace_back([&cache, &failed, thread]() {
			for (int iteration = 0; iteration < 2000; ++iteration) {
				const double a = static_cast<double>((iteration + thread) % 31);
				const double b = static_cast<double>((iteration * 2 + thread) % 23);
				const double c = static_cast<double>((iteration * 5 + thread) % 17);
				if (cache.getValue(a, b, c) != a + 2.0 * b - c) failed = true;
			}
		});
	}
	for (auto &worker : workers) worker.join();
	EXPECT_FALSE(failed.load());
}

TEST(CacheTools, ReplacingFunctionDoesNotCacheStaleInFlightResult) {
	CacheStorageWith2Args<double, double, double> cache;
	std::promise<void> startedPromise;
	std::future<void> started = startedPromise.get_future();
	std::promise<void> releasePromise;
	std::shared_future<void> release = releasePromise.get_future().share();
	cache.setFunction([&](double, double) {
		startedPromise.set_value();
		release.wait();
		return 1.0;
	});

	double inFlightResult = 0;
	std::thread worker([&]() { inFlightResult = cache.getValue(1.0, 2.0); });
	started.wait();
	cache.setFunction([](double, double) { return 2.0; });
	releasePromise.set_value();
	worker.join();

	EXPECT_DOUBLE_EQ(inFlightResult, 1.0);
	EXPECT_DOUBLE_EQ(cache.getValue(1.0, 2.0), 2.0);
}

TEST(CacheTools, InverseComptonCacheIncludesEnergyInItsKey) {
	CacheStorageIC2 cache;
	cache.setFunction([](int component, int bin, QEnergy energy) {
		return QGREmissivity(component + bin + static_cast<double>(energy / 1_GeV));
	});

	EXPECT_NE(cache.getValue(1, 2, 3_GeV), cache.getValue(1, 2, 4_GeV));
	EXPECT_EQ(cache.getValue(1, 2, 3_GeV), QGREmissivity(6));
	EXPECT_EQ(cache.getValue(1, 2, 4_GeV), QGREmissivity(7));
}

TEST(CacheTools, Kamae06GammaCorrectness) {
	auto result = runKamae06GammaBenchmark();
	EXPECT_DOUBLE_EQ(static_cast<double>(result.integral_cached), static_cast<double>(result.integral_noncached));
}

TEST(CacheTools, MovingCachedInteractionsRebindsOwningCallbacks) {
	interactions::Kamae06Gamma gammaSource;
	gammaSource.setCachingStorage(std::make_unique<CacheStorageCrossSection>());
	interactions::Kamae06Gamma gamma(std::move(gammaSource));
	EXPECT_EQ(gamma.getDiffCrossSection(10_GeV, 1_GeV),
	          gamma.getDiffCrossSectionDirectly(10_GeV, 1_GeV));

	interactions::BremsstrahlungGALPROP bremsstrahlungSource;
	interactions::BremsstrahlungGALPROP bremsstrahlung(
	    std::move(bremsstrahlungSource));
	const auto target = interactions::BremsstrahlungAbstract::Target::HI;
	const QDiffCrossSection cached =
	    bremsstrahlung.getDiffCrossSectionForTarget(target, 100_MeV, 1_MeV);
	bremsstrahlung.disableCaching();
	EXPECT_EQ(cached,
	          bremsstrahlung.getDiffCrossSectionForTarget(target, 100_MeV, 1_MeV));
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

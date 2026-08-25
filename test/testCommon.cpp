#include <memory>
#include <thread>
#include <vector>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

TEST(Common, toGalCoord) {
	auto gal_dir_1 = toGalCoord(QDirection({90_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(0_deg), static_cast<double>(gal_dir_1[0]));

	auto gal_dir_2 = toGalCoord(QDirection({179_deg, 270_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(-89_deg), static_cast<double>(gal_dir_2[0]));
	EXPECT_DOUBLE_EQ(static_cast<double>(270_deg), static_cast<double>(gal_dir_2[1]));

	auto gal_dir_3 = toGalCoord(QDirection({1_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(89_deg), static_cast<double>(gal_dir_3[0]));

	auto gal_dir_4 = toGalCoord(QDirection({100_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(-10_deg), static_cast<double>(gal_dir_4[0]));

	auto gal_dir_5 = toGalCoord(QDirection({200_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(70_deg), static_cast<double>(gal_dir_5[0]));

	auto gal_dir_6 = toGalCoord(QDirection({270_deg, 710_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(0_deg), static_cast<double>(gal_dir_6[0]));
	EXPECT_DOUBLE_EQ(static_cast<double>(350_deg), static_cast<double>(gal_dir_6[1]));
}

TEST(Common, fromGalCoord) {
	auto gal_dir_1 = fromGalCoord(QDirection({90_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(0_deg), static_cast<double>(gal_dir_1[0]));

	auto gal_dir_2 = fromGalCoord(QDirection({89_deg, 270_deg}));
	EXPECT_NEAR(static_cast<double>(1_deg), static_cast<double>(gal_dir_2[0]), 1e-8);
	EXPECT_DOUBLE_EQ(static_cast<double>(270_deg), static_cast<double>(gal_dir_2[1]));

	auto gal_dir_3 = fromGalCoord(QDirection({1_deg, 0_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(89_deg), static_cast<double>(gal_dir_3[0]));

	auto gal_dir_4 = fromGalCoord(QDirection({-10_deg, 10_deg}));
	EXPECT_DOUBLE_EQ(static_cast<double>(100_deg), static_cast<double>(gal_dir_4[0]));
}

TEST(Common, galacticBorder) {
	Vector3QLength observerPosition(8.5_kpc, 0, 0);
	QDirection direction;

	// pointing towards the galactic centre
	direction[0] = 90_deg;
	direction[1] = 0_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(observerPosition, direction, 20_kpc)),
	            static_cast<double>(28.5_kpc), static_cast<double>(1_pc));
	// away from the galactic centre
	direction[0] = 90_deg;
	direction[1] = 180_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(observerPosition, direction, 20_kpc)),
	            static_cast<double>(11.5_kpc), static_cast<double>(1_pc));
	// left
	direction[0] = 90_deg;
	direction[1] = 90_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(observerPosition, direction, 20_kpc)),
	            static_cast<double>(18.103_kpc), static_cast<double>(10_pc));
	// right
	direction[0] = 90_deg;
	direction[1] = 270_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(observerPosition, direction, 20_kpc)),
	            static_cast<double>(18.103_kpc), static_cast<double>(10_pc));
	// the galactic north
	direction[0] = 0_deg;
	direction[1] = 0_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(observerPosition, direction, 20_kpc)),
	            static_cast<double>(5.0_kpc), static_cast<double>(10_pc));
	// spherical border: (18.103_kpc);
	// the galactic south
	direction[0] = 180_deg;
	direction[1] = 0_deg;
	EXPECT_NEAR(static_cast<double>(distanceToGalBorder(observerPosition, direction, 20_kpc)),
	            static_cast<double>(5.0_kpc), static_cast<double>(10_pc));
	// spherical border: (18.103_kpc);
}

TEST(Common, GalacticPositionIncludesAllObserverCoordinates) {
	const Vector3QLength observer(8_kpc, 2_kpc, 1_kpc);
	const QLength tolerance = 1e-12 * 1_kpc;

	const auto towardCentre = getGalacticPosition(observer, 1_kpc, QDirection({90_deg, 0_deg}));
	EXPECT_NEAR(static_cast<double>(towardCentre.x), static_cast<double>(7_kpc), static_cast<double>(tolerance));
	EXPECT_NEAR(static_cast<double>(towardCentre.y), static_cast<double>(2_kpc), static_cast<double>(tolerance));
	EXPECT_NEAR(static_cast<double>(towardCentre.z), static_cast<double>(1_kpc), static_cast<double>(tolerance));

	const auto transverse = getGalacticPosition(observer, 1_kpc, QDirection({90_deg, 90_deg}));
	EXPECT_NEAR(static_cast<double>(transverse.x), static_cast<double>(8_kpc), static_cast<double>(tolerance));
	EXPECT_NEAR(static_cast<double>(transverse.y), static_cast<double>(1_kpc), static_cast<double>(tolerance));
	EXPECT_NEAR(static_cast<double>(transverse.z), static_cast<double>(1_kpc), static_cast<double>(tolerance));

	const auto north = getGalacticPosition(observer, 1_kpc, QDirection({0_deg, 0_deg}));
	EXPECT_NEAR(static_cast<double>(north.x), static_cast<double>(8_kpc), static_cast<double>(tolerance));
	EXPECT_NEAR(static_cast<double>(north.y), static_cast<double>(2_kpc), static_cast<double>(tolerance));
	EXPECT_NEAR(static_cast<double>(north.z), static_cast<double>(2_kpc), static_cast<double>(tolerance));
}

TEST(Common, ThreadChunkingHandlesEmptyAndSmallQueues) {
	EXPECT_GE(getThreadsNumber(), 1u);
	EXPECT_TRUE(getThreadChunks(0).empty());
	EXPECT_TRUE(getIndexedThreadChunks({}).empty());

	const auto chunks = getThreadChunks(3);
	ASSERT_FALSE(chunks.empty());
	EXPECT_LE(chunks.size(), 3u);
	unsigned int next = 0;
	for (const auto &chunk : chunks) {
		EXPECT_EQ(chunk.first, next);
		EXPECT_GT(chunk.second, chunk.first);
		next = chunk.second;
	}
	EXPECT_EQ(next, 3u);

	const auto indexed = getIndexedThreadChunks({2, 4, 6});
	ASSERT_FALSE(indexed.empty());
	EXPECT_LE(indexed.size(), 3u);
	std::size_t count = 0;
	for (const auto &chunk : indexed) count += chunk.size();
	EXPECT_EQ(count, 3u);
}

TEST(Common, DataPathResolutionIsThreadSafe) {
	const std::string filename = "Interactions/AAfrag/QGSJET_II-04m_gamma.p00";
	const std::string expected = getDataPath(filename);
	std::vector<std::string> paths(8);
	std::vector<std::thread> workers;
	workers.reserve(paths.size());
	for (std::size_t i = 0; i < paths.size(); ++i) {
		workers.emplace_back([&, i]() { paths[i] = getDataPath(filename); });
	}
	for (auto &worker : workers) worker.join();
	for (const auto &path : paths) EXPECT_EQ(path, expected);
}

TEST(Common, intensityToTemperature) {
	// T = I * c^2 / (2 * nu^2 * k_boltzmann)
	QIntensity intensity(1);
	QTemperature temp = intensityToTemperature(intensity, 1_Hz);
	EXPECT_NEAR(static_cast<double>(temp), 3.2548e39, 1e36);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}  // namespace hermes

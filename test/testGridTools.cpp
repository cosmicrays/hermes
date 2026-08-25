#include <cmath>
#include <memory>

#include "gtest/gtest.h"
#include "hermes.h"

namespace hermes {

TEST(GridTools, ScalarStatisticsAndScalingUsePublicSharedPointerAPI) {
	auto grid = std::make_shared<ScalarGrid>(Vector3d(0.0), 2, 1.0);
	for (std::size_t i = 0; i < grid->getGridSize(); ++i)
		grid->get(i) = static_cast<float>(i + 1);

	EXPECT_DOUBLE_EQ(meanFieldStrength(grid), 4.5);
	EXPECT_DOUBLE_EQ(rmsFieldStrength(grid), std::sqrt(25.5));
	scaleGrid(grid, 2.0);
	EXPECT_DOUBLE_EQ(meanFieldStrength(grid), 9.0);
	EXPECT_DOUBLE_EQ(rmsFieldStrength(grid), 2.0 * std::sqrt(25.5));
}

TEST(GridTools, VectorStatisticsAndScalingAreConsistent) {
	auto grid = std::make_shared<VectorGrid>(Vector3d(0.0), 2, 1.0);
	for (std::size_t i = 0; i < grid->getGridSize(); ++i)
		grid->get(i) = Vector3f(1, 2, 2);

	const Vector3f mean = meanFieldVector(grid);
	EXPECT_FLOAT_EQ(mean.x, 1.0f);
	EXPECT_FLOAT_EQ(mean.y, 2.0f);
	EXPECT_FLOAT_EQ(mean.z, 2.0f);
	EXPECT_DOUBLE_EQ(meanFieldStrength(grid), 3.0);
	EXPECT_DOUBLE_EQ(rmsFieldStrength(grid), 3.0);
	scaleGrid(grid, 2.0);
	EXPECT_DOUBLE_EQ(meanFieldStrength(grid), 6.0);
	EXPECT_DOUBLE_EQ(rmsFieldStrength(grid), 6.0);
}

}  // namespace hermes

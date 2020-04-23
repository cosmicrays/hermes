#include <utility>

#include "hermes/magneticfields/MagneticFieldGrid.h"

namespace hermes {
namespace magneticfields {

MagneticFieldGrid::MagneticFieldGrid(std::shared_ptr<VectorGrid> grid) {
    setGrid(std::move(grid));
}

void MagneticFieldGrid::setGrid(std::shared_ptr<VectorGrid> grid) {
    this->grid = std::move(grid);
}

std::shared_ptr<VectorGrid> MagneticFieldGrid::getGrid() { return grid; }

Vector3d MagneticFieldGrid::getField(const Vector3d &pos) const {
    return grid->interpolate(pos);
}

ModulatedMagneticFieldGrid::ModulatedMagneticFieldGrid(
    const std::shared_ptr<VectorGrid>& grid, const std::shared_ptr<ScalarGrid>& modGrid) {
    grid->setReflective(false);
    modGrid->setReflective(true);
    setGrid(grid);
    setModulationGrid(modGrid);
}

void ModulatedMagneticFieldGrid::setGrid(std::shared_ptr<VectorGrid> g) {
    grid = std::move(g);
}

std::shared_ptr<VectorGrid> ModulatedMagneticFieldGrid::getGrid() {
    return grid;
}

void ModulatedMagneticFieldGrid::setModulationGrid(
    std::shared_ptr<ScalarGrid> g) {
    modGrid = std::move(g);
}

std::shared_ptr<ScalarGrid> ModulatedMagneticFieldGrid::getModulationGrid() {
    return modGrid;
}

void ModulatedMagneticFieldGrid::setReflective(bool gridReflective,
					       bool modGridReflective) {
    grid->setReflective(gridReflective);
    modGrid->setReflective(modGridReflective);
}

Vector3d ModulatedMagneticFieldGrid::getField(const Vector3d &pos) const {
    float m = modGrid->interpolate(pos);
    Vector3d b = grid->interpolate(pos);
    return b * m;
}

} // namespace magneticfields
} // namespace hermes

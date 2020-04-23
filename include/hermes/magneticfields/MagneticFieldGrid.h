#ifndef CRPROPA_MAGNETICFIELDGRID_H
#define CRPROPA_MAGNETICFIELDGRID_H

#include "hermes/Grid.h"
#include "hermes/magneticfields/MagneticField.h"

namespace hermes {
namespace magneticfields {
/**
 * \addtogroup MagneticFields
 * @{
 */

/**
 @class MagneticFieldGrid
 @brief Magnetic field on a periodic (or reflective), cartesian grid with
 trilinear interpolation.

 This class wraps a VectorGrid to serve as a MagneticField.
 */
class MagneticFieldGrid : public MagneticField {
    std::shared_ptr<VectorGrid> grid;

  public:
    MagneticFieldGrid(std::shared_ptr<VectorGrid> grid);
    void setGrid(std::shared_ptr<VectorGrid> grid);
    std::shared_ptr<VectorGrid> getGrid();
    Vector3d getField(const Vector3d &position) const;
};

/**
 @class ModulatedMagneticFieldGrid
 @brief Modulated magnetic field on a periodic grid.

 This class wraps a VectorGrid to serve as a MagneticField.
 The field is modulated on-the-fly with a ScalarGrid.
 The VectorGrid and ScalarGrid do not need to share the same origin, spacing or
 size.
 */
class ModulatedMagneticFieldGrid : public MagneticField {
    std::shared_ptr<VectorGrid> grid;
    std::shared_ptr<ScalarGrid> modGrid;

  public:
    ModulatedMagneticFieldGrid() {}
    ModulatedMagneticFieldGrid(const std::shared_ptr<VectorGrid>& grid,
			       const std::shared_ptr<ScalarGrid>& modGrid);
    void setGrid(std::shared_ptr<VectorGrid> grid);
    void setModulationGrid(std::shared_ptr<ScalarGrid> modGrid);
    std::shared_ptr<VectorGrid> getGrid();
    std::shared_ptr<ScalarGrid> getModulationGrid();
    void setReflective(bool gridReflective, bool modGridReflective);
    Vector3d getField(const Vector3d &position) const;
};
/** @} */
} // namespace magneticfields
} // namespace hermes

#endif // CRPROPA_MAGNETICFIELDGRID_H

#ifndef HERMES_TABULATEDCR_H
#define HERMES_TABULATEDCR_H

#include <string>
#include <vector>

#include "hermes/cosmicrays/CosmicRayDensity.h"

namespace hermes { namespace cosmicrays {
/**
 * \addtogroup CosmicRays
 * @{
 */

/**
 * Proton cosmic-ray density read from a user-provided two-column table.
 *
 * Each non-empty, non-comment line must contain a positive energy followed by
 * a positive differential intensity. Lines beginning with `#`, and inline
 * comments introduced by `#`, are ignored. Energies must be strictly
 * increasing. The filename is used exactly as supplied and is not resolved
 * through the HERMES data directory.
 *
 * The convenience constructor interprets the columns as GeV and
 * (GeV cm^2 s sr)^-1. The unit-aware constructor accepts alternative column
 * units. Intensities are assumed isotropic and converted to differential
 * number density using n(E) = 4 pi I(E) / [beta(E) c], where E is proton
 * kinetic energy. The intensity is interpolated in log-log space before this
 * conversion and is zero outside the tabulated energy range.
 *
 * The spatial profile is radially uniform and Gaussian in height,
 * exp[-z^2 / (2 sigma_z^2)], normalized to one in the Galactic plane. The
 * logarithmic integration axis spans the table and is independent of the
 * original sampling.
 */
class TabulatedCR : public CosmicRayDensity {
  private:
	std::vector<QEnergy> tabulatedEnergies;
	std::vector<QDiffIntensity> tabulatedIntensities;
	QLength sigmaZ;

	void loadTable(const std::string &filename, const QEnergy &energyUnit, const QDiffIntensity &intensityUnit);
	void makeEnergyRange(int integrationSteps);
	QDiffIntensity interpolateIntensity(const QEnergy &energy) const;

  public:
	static constexpr int DefaultIntegrationSteps = 100;

	TabulatedCR(const std::string &filename, const QLength &sigmaZ = 1_kpc,
	            int integrationSteps = DefaultIntegrationSteps);
	TabulatedCR(const std::string &filename, const QEnergy &energyUnit, const QDiffIntensity &intensityUnit,
	            const QLength &sigmaZ = 1_kpc, int integrationSteps = DefaultIntegrationSteps);

	QPDensityPerEnergy getDensityPerEnergy(const QEnergy &energy, const Vector3QLength &position) const override;

	QLength getSigmaZ() const { return sigmaZ; }
	std::vector<QEnergy> getTabulatedEnergyAxis() const { return tabulatedEnergies; }
};

/** @}*/
}}  // namespace hermes::cosmicrays

#endif  // HERMES_TABULATEDCR_H

#ifndef HERMES_KAMAE06NEUTRINO_H
#define HERMES_KAMAE06NEUTRINO_H

/*
https://github.com/niklask/cparamlib

cparamlib is a simple library for calculation of fluxes for stable secondary
particles created in proton-proton interactions in astronomical environment
according to the model described in Tuneyoshi Kamae, Niklas Karlsson, Tsunefumi
Mizuno, Toshinori Abe, and Tatsumi Koi; "Parameterization of $\gamma$, $e^{\pm}$
and Neutrino Spectra Produced by $p-p$ Interaction in Astronomical Environment"
to appear in Astrophysical Journal (2006) and
  Niklas Karlsson and Tuneyoshi Kamae;
  "Parameterization of the Angular Distribution of Gamma Rays Produced by P-P
Interaction in Astronomical Environment" to by submitted to Astrophysical
Journal (2007).
*/

#include <memory>

#include "hermes/CacheTools.h"
#include "hermes/interactions/DiffCrossSection.h"

extern "C" {
#include "cparamlib.h"
}

namespace hermes { namespace interactions {
/**
 * \addtogroup Interactions
 * @{
 */

class Kamae06Neutrino : public DifferentialCrossSection {
  private:
	std::unique_ptr<CacheStorageCrossSection> cache;

  public:
	Kamae06Neutrino();
	void setCachingStorage(std::unique_ptr<CacheStorageCrossSection> cache);

	QDiffCrossSection getDiffCrossSection(const QEnergy &E_proton, const QEnergy &E_nu) const override;
	QDiffCrossSection getDiffCrossSection(const PID &projectile, const PID &target, const QEnergy &E_proj,
	                                      const QEnergy &E_secondary) const override;

	QDiffCrossSection getDiffCrossSectionDirectly(const QEnergy &E_proton, const QEnergy &E_nu) const;
};

/** @}*/
}}  // namespace hermes::interactions

#endif  // HERMES_KAMAE06NEUTRINO_H

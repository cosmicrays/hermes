#ifndef HERMES_AAFRAGGAMMA_H
#define HERMES_AAFRAGGAMMA_H

/*
Model references
(1) S. Koldobskiy, M. Kachelrieß, A. Lskavyan, A. Neronov, S. Ostapchenko, and D. V. Semikoz, “Energy spectra of
secondaries in proton-proton interactions,” Phys. Rev. D, vol. 104, no. 12, p. 123027, 2021, arXiv:2110.00496
(2) M. Kachelrieß, I. V. Moskalenko, and S. Ostapchenko, “AAfrag: Interpolation routines for Monte Carlo results on
secondary production in proton-proton, proton-nucleus and nucleus-nucleus interactions,” Comput. Phys. Commun., vol.
245, p. 106846, 2019.
*/

#include <memory>

#include "hermes/interactions/DiffCrossSection.h"

namespace hermes { namespace interactions {
/**
 * \addtogroup Interactions
 * @{
 */

class AAfragGamma : public DifferentialCrossSection {
  public:
	AAfragGamma();
	QDiffCrossSection getDiffCrossSection(const QEnergy &E_proton, const QEnergy &E_gamma) const override;
};

/** @}*/
}}  // namespace hermes::interactions

#endif  // HERMES_AAFRAGGAMMA_H

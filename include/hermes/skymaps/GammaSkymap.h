#ifndef HERMES_GAMMASKYMAP_H
#define HERMES_GAMMASKYMAP_H

#include "hermes/skymaps/SkymapTemplate.h"

namespace hermes {
/**
 * \addtogroup Skymaps
 * @{
 */

/**
 @class GammaSkymap
 @brief A skymap container suitable for gamma-ray emissions; saves pixels in
 units of differential intensity (GeV^-1 m^-2 s^-1 sr^-1), specified by
 gamma-ray energy (J).
 */
class GammaSkymap : public SkymapTemplate<QDifferentialIntensity, QEnergy> {
  public:
    GammaSkymap(std::size_t nside_, QEnergy Egamma_)
	: SkymapTemplate(nside_, Egamma_) {
	initDefaultOutputUnits(1 / (1_GeV * 1_m2 * 1_s * 1_sr),
			       "GeV^-1 m^-2 s^-1 sr^-1");
    };

    void setEnergy(QEnergy Egamma_) { setSkymapParameter(Egamma_); }
    QEnergy getEnergy() const { return skymapParameter; }

    void computePixel(
	std::size_t ipix,
	std::shared_ptr<IntegratorTemplate<QDifferentialIntensity, QEnergy>>
	    integrator_) {
	fluxContainer[ipix] = integrator_->integrateOverLOS(
	    pix2ang_ring(getNside(), ipix), skymapParameter);
    }
};

/** @}*/
} // namespace hermes
#endif // HERMES_GAMMASKYMAP_H

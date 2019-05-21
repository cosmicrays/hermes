#ifndef HERMES_INTENSITYSKYMAP_H
#define HERMES_INTENSITYSKYMAP_H

#include "hermes/skymaps/SkymapTemplate.h"

namespace hermes {

class DiffFluxSkymap: public SkymapTemplate<QDifferentialFlux> {
private:
	QEnergy Egamma;
public:
	DiffFluxSkymap(std::size_t nside_, QEnergy Egamma_) : SkymapTemplate(nside_), Egamma(Egamma_) { };

	void setEnergy(QEnergy Egamma_) {
		Egamma = Egamma_;
	}
	QEnergy getEnergy() const {
		return Egamma;
	}
	void computePixel(
		std::size_t ipix,
		std::shared_ptr<IntegratorTemplate<QDifferentialFlux> > integrator_) {
		iterdir = pix2ang_ring(getNside(), ipix);
		fluxContainer[ipix] = integrator_->integrateOverLOS(iterdir, Egamma);
	}
};

} // namespace hermes

#endif // HERMES_INTENSITYSKYMAP_H

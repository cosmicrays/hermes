#ifndef HERMES_GAMMASKYMAP_H
#define HERMES_GAMMASKYMAP_H

#include "hermes/skymaps/SkymapTemplate.h"

namespace hermes {

class GammaSkymap: public SkymapTemplate<QDifferentialIntensity, QEnergy> {
private:
	QEnergy Egamma;
public:
	GammaSkymap(std::size_t nside_, QEnergy Egamma_) :
		SkymapTemplate(nside_),
		Egamma(Egamma_) {
	
		initDefaultUnits(1/(1_GeV*1_m2*1_s*1_sr), "GeV^-1 m^-2 s^-1 sr^-1");	
	};

	void setEnergy(QEnergy Egamma_) {
		Egamma = Egamma_;
	}
	QEnergy getEnergy() const {
		return Egamma;
	}
	void computePixel(
		std::size_t ipix,
		std::shared_ptr<IntegratorTemplate<QDifferentialIntensity, QEnergy> > integrator_) {
		iterdir = pix2ang_ring(getNside(), ipix);
		fluxContainer[ipix] = integrator_->integrateOverLOS(iterdir, Egamma);
	}
};

} // namespace hermes

#endif // HERMES_GAMMASKYMAP_H

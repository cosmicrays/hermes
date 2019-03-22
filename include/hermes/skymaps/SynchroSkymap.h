#ifndef HERMES_SYNCHROSKYMAP_H
#define HERMES_SYNCHROSKYMAP_H

#include <hermes/skymaps/SkymapTemplate.h>
#include <hermes/integrators/SynchroIntegrator.h>

namespace hermes {

class SynchroSkymap: public SkymapTemplate<QTemperature> {
private:
	QFrequency freq;
public:
	SynchroSkymap(std::size_t nside_, QFrequency freq_) : SkymapTemplate(nside_), freq(freq_) { };

	void setFrequency(QFrequency freq_) {
		freq = freq_;
	}
	QFrequency getFrequency() {
		return freq;
	}
	void computePixel(
		std::size_t ipix,
		std::shared_ptr<IntegratorTemplate<QTemperature> > integrator_) {
		iterdir = pix2ang_ring(getNside(), ipix);
		fluxContainer[ipix] = integrator_->integrateOverLOS(iterdir, freq);
	}
};

} // namespace hermes

#endif // HERMES_SYNCHROSKYMAP_H

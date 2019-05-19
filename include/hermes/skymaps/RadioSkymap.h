#ifndef HERMES_RADIOSKYMAP_H
#define HERMES_RADIOSKYMAP_H

#include "hermes/skymaps/SkymapTemplate.h"

namespace hermes {

class RadioSkymap: public SkymapTemplate<QTemperature> {
private:
	QFrequency freq;
public:
	RadioSkymap(std::size_t nside_, QFrequency freq_) : SkymapTemplate(nside_), freq(freq_) { };

	void setFrequency(QFrequency freq_) {
		freq = freq_;
	}
	QFrequency getFrequency() const {
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

#endif // HERMES_RADIOSKYMAP_H

#ifndef HERMES_GENERICSKYMAP_H
#define HERMES_GENERICSKYMAP_H

#include <hermes/skymaps/SkymapTemplate.h>
#include <hermes/integrators/GenericIntegrator.h>

namespace hermes {

class GenericSkymap: public SkymapTemplate<QNumber> {
private:
	QFrequency freq;
public:
	GenericSkymap(std::size_t nside_, QFrequency freq_) : SkymapTemplate(nside_), freq(freq_) { };

	void setFrequency(QFrequency freq_) {
		freq = freq_;
	}
	QFrequency getFrequency() const {
		return freq;
	}
	void computePixel(
		std::size_t ipix,
		std::shared_ptr<IntegratorTemplate<QNumber> > integrator_) {
		iterdir = pix2ang_ring(getNside(), ipix);
		fluxContainer[ipix] = integrator_->integrateOverLOS(iterdir, freq);
	}
};

} // namespace hermes

#endif // HERMES_GENERICSKYMAP_H

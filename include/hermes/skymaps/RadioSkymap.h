#ifndef HERMES_RADIOSKYMAP_H
#define HERMES_RADIOSKYMAP_H

#include "hermes/skymaps/SkymapTemplate.h"

namespace hermes {

/**
 * \addtogroup Skymaps
 * @{
 */

/**
 @class RadioSkymap
 @brief A skymap container suitable for radio emissions; saves pixels in units
 of temperature (K), specified by frequency (Hz).
 */
class RadioSkymap : public SkymapTemplate<QTemperature, QFrequency> {
  public:
	RadioSkymap(std::size_t nside, QFrequency freq)
	    : SkymapTemplate(nside, freq, SkymapDefinitions("FREQ")){
	};

	void setFrequency(QFrequency freq) {
		setSkymapParameter(freq);
	}
	QFrequency getFrequency() const { return skymapParameter; }
	void computePixel(
	    std::size_t ipix,
	    const std::shared_ptr<IntegratorTemplate<QTemperature, QFrequency>>
	        &integrator_) override {
		fluxContainer[ipix] = integrator_->integrateOverLOS(
		    pix2ang_ring(getNside(), ipix), skymapParameter);
	}
};

/** @}*/
}  // namespace hermes
#endif  // HERMES_RADIOSKYMAP_H

#ifndef HERMES_SYNCHROSKYMAP_H
#define HERMES_SYNCHROSKYMAP_H

#include <hermes/skymaps/SkymapTemplate.h>

namespace hermes {

class SynchroSkymap: public SkymapTemplate<QTemperature> {
private:
	QFrequency freq;
public:
	SynchroSkymap(std::size_t nside_, QFrequency freq_) {
		setNside(nside_);
		setFrequency(freq_);
		initContainer();
	}
	void setFrequency(QFrequency freq_) {
		freq = freq_;
	}
	QFrequency getFrequency() {
		return freq;
	}
};

} // namespace hermes

#endif // HERMES_SYNCHROSKYMAP_H

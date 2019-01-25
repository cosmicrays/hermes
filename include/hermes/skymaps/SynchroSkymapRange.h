#ifndef HERMES_SYNCHROSKYMAPRANGE_H
#define HERMES_SYNCHROSKYMAPRANGE_H

#include <hermes/skymaps/SynchroSkymap.h>

namespace hermes {

class SynchroSkymapRange {
private:
	std::vector<SynchroSkymap> skymaps;
	std::vector<QFrequency> freqs;
	QFrequency minFreq, maxFreq;
	std::size_t nside;
	int freqSteps;
	void initFrequencyRange();
public:
	SynchroSkymapRange(std::size_t nside_, QFrequency minFreq_, QFrequency maxFreq_, int freqSteps_);
	~SynchroSkymapRange();
};

} // namespace hermes

#endif // HERMES_SYNCHROSKYMAPRANGE_H

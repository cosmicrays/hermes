#include "hermes/skymaps/SynchroSkymapRange.h"

#include <iostream>
#include <stdexcept>

namespace hermes {

SynchroSkymapRange::SynchroSkymapRange(
		std::size_t nside_, QFrequency minFreq_,
		QFrequency maxFreq_, int freqSteps_) :
		nside(nside_), minFreq(minFreq_),
		maxFreq(maxFreq_), freqSteps(freqSteps_) {
	initFrequencyRange();
}

SynchroSkymapRange::~SynchroSkymapRange() {
}

void SynchroSkymapRange::initFrequencyRange() {
	for (QFrequency f = minFreq; f <= maxFreq; f += (maxFreq-minFreq)/freqSteps ) {
		freqs.push_back(f);
		skymaps.push_back(SynchroSkymap(nside, f));
	}
}

} // namespace hermes

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

void SynchroSkymapRange::setIntegrator(std::shared_ptr<IntegratorTemplate<QTemperature> > integrator_) {
	for(iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		it->setIntegrator(integrator_);
	}
}

void SynchroSkymapRange::compute() {
	for(iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		it->compute();
	}
}

void SynchroSkymapRange::save(std::shared_ptr<Output> output) const {
	
	output->initOutput();
	
	for(const_iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		int npix = static_cast<int>((it)->getNpix());

		output->createTable(npix);
		output->writeMetadata(it->getNside(), it->getRes(), it->getDescription());

		float tempArray[npix];
		for (unsigned long i = 0; i < npix; ++i)
			tempArray[i] = static_cast<float>(it->operator[](i));

		output->writeColumn(npix, tempArray);
	}
}

SynchroSkymapRange::iterator SynchroSkymapRange::begin() {
        return skymaps.begin();
}

SynchroSkymapRange::const_iterator SynchroSkymapRange::begin() const {
        return skymaps.begin();
}

SynchroSkymapRange::iterator SynchroSkymapRange::end() {
        return skymaps.end();
}

SynchroSkymapRange::const_iterator SynchroSkymapRange::end() const {
        return skymaps.end();
}

} // namespace hermes

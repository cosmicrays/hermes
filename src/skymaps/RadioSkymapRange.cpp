#include "hermes/skymaps/RadioSkymapRange.h"

#include <iostream>
#include <stdexcept>

namespace hermes {

RadioSkymapRange::RadioSkymapRange(
		std::size_t nside_, QFrequency minFreq_,
		QFrequency maxFreq_, int freqSteps_) :
		nside(nside_), minFreq(minFreq_),
		maxFreq(maxFreq_), freqSteps(freqSteps_) {
	initFrequencyRange();
}

RadioSkymapRange::~RadioSkymapRange() {
}

void RadioSkymapRange::initFrequencyRange() {
	
	double scaleFactor = std::pow(static_cast<double>(maxFreq/minFreq),
					1.0/(freqSteps-1));

	QFrequency f;
	for (int i = 0; i < freqSteps; ++i) {
		f = std::pow(scaleFactor, i) * minFreq;
		freqs.push_back(f);
		skymaps.push_back(RadioSkymap(nside, f));
	}
}

void RadioSkymapRange::setIntegrator(std::shared_ptr<IntegratorTemplate<QTemperature> > integrator_) {
	for(iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		it->setIntegrator(integrator_);
	}
}

void RadioSkymapRange::compute() {
	for(iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
       		std::cout << "hermes::SkymapRante: "
			  << it - skymaps.begin() << "/" << skymaps.size()
			  << ", freq = " << it->getFrequency() << " Hz" << std::endl;
		it->compute();
	}
}

void RadioSkymapRange::save(std::shared_ptr<Output> output) const {
	
	output->initOutput();
	
	for(const_iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		int npix = static_cast<int>((it)->getNpix());

		output->createTable(npix);
		output->writeMetadata(it->getNside(), it->getRes(), it->getDescription());
		output->writeKeyValueAsDouble(std::string("FREQ"),
			static_cast<double>(it->getFrequency()),
			std::string("The skymap radio frequency in Hz."));

		float tempArray[npix];
		for (unsigned long i = 0; i < npix; ++i)
			tempArray[i] = static_cast<float>(it->operator[](i));

		output->writeColumn(npix, tempArray);
	}
}

RadioSkymapRange::iterator RadioSkymapRange::begin() {
        return skymaps.begin();
}

RadioSkymapRange::const_iterator RadioSkymapRange::begin() const {
        return skymaps.begin();
}

RadioSkymapRange::iterator RadioSkymapRange::end() {
        return skymaps.end();
}

RadioSkymapRange::const_iterator RadioSkymapRange::end() const {
        return skymaps.end();
}

} // namespace hermes

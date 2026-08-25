#include "hermes/skymaps/RadioSkymapRange.h"

#include <iostream>
#include <stdexcept>

namespace hermes {

RadioSkymapRange::RadioSkymapRange(std::size_t nside_, QFrequency minFreq_,
                                   QFrequency maxFreq_, int freqSteps_)
    : nside(nside_),
      minFreq(minFreq_),
      maxFreq(maxFreq_),
      freqSteps(freqSteps_) {
	if (freqSteps < 2)
		throw std::invalid_argument("RadioSkymapRange requires at least two frequency steps");
	if (!(minFreq > QFrequency(0)))
		throw std::invalid_argument("RadioSkymapRange minimum frequency must be positive");
	if (maxFreq < minFreq)
		throw std::invalid_argument("RadioSkymapRange maximum frequency must not be smaller than the minimum");
	initFrequencyRange();
}

RadioSkymapRange::~RadioSkymapRange() {}

void RadioSkymapRange::initFrequencyRange() {
	double scaleFactor =
	    std::pow(static_cast<double>(maxFreq / minFreq), 1.0 / (freqSteps - 1));

	QFrequency f;
	for (int i = 0; i < freqSteps; ++i) {
		f = (i == freqSteps - 1) ? maxFreq : std::pow(scaleFactor, i) * minFreq;
		freqs.push_back(f);
		skymaps.push_back(RadioSkymap(nside, f));
	}
}

void RadioSkymapRange::setIntegrator(
    const std::shared_ptr<IntegratorTemplate<QTemperature, QFrequency>>&
        integrator_) {
	for (iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		it->setIntegrator(integrator_);
	}
}

void RadioSkymapRange::setMask(const std::shared_ptr<SkymapMask>& mask_) {
	for (iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		it->setMask(mask_);
	}
}

std::size_t RadioSkymapRange::size() const { return skymaps.size(); }

RadioSkymap RadioSkymapRange::operator[](std::size_t i) const {
	return skymaps.at(i);
}

const std::vector<QFrequency> &RadioSkymapRange::getFrequencies() const {
	return freqs;
}

void RadioSkymapRange::compute() {
	for (iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		std::cout << "hermes::SkymapRange: " << it - skymaps.begin() + 1 << "/"
		          << skymaps.size() << ", Frequency = " << it->getFrequency()
		          << " Hz" << std::endl;
		it->compute();
	}
}

void RadioSkymapRange::save(
    const std::shared_ptr<outputs::Output>& output) const {
	for (const_iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		int npix = static_cast<int>((it)->getNpix());

		output->createTable(npix, it->getOutputUnitsAsString());
		output->writeMetadata(it->getNside(), it->getRes(), it->hasMask(),
		                      it->getDescription());
		output->writeKeyValueAsDouble(
		    std::string("FREQ"), static_cast<double>(it->getFrequency()),
		    std::string("Skymap frequency/energy/... (in SI base unit)"));
		auto tempArray = it->containerToRawVector();
		output->writeColumn(npix, tempArray.data());
	}
}

RadioSkymapRange::iterator RadioSkymapRange::begin() { return skymaps.begin(); }

RadioSkymapRange::const_iterator RadioSkymapRange::begin() const {
	return skymaps.begin();
}

RadioSkymapRange::iterator RadioSkymapRange::end() { return skymaps.end(); }

RadioSkymapRange::const_iterator RadioSkymapRange::end() const {
	return skymaps.end();
}

}  // namespace hermes

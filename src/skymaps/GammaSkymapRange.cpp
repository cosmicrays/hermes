#include "hermes/skymaps/GammaSkymapRange.h"

#include <iostream>
#include <stdexcept>

namespace hermes {

GammaSkymapRange::GammaSkymapRange(std::size_t nside_, QEnergy minEn_,
                                   QEnergy maxEn_, int enSteps_)
    : nside(nside_), minEn(minEn_), maxEn(maxEn_), enSteps(enSteps_) {
	initEnergyRange();
}

GammaSkymapRange::~GammaSkymapRange() {}

void GammaSkymapRange::initEnergyRange() {
	double scaleFactor =
	    std::pow(static_cast<double>(maxEn / minEn), 1.0 / (enSteps - 1));

	QEnergy e;
	for (int i = 0; i < enSteps; ++i) {
		e = std::pow(scaleFactor, i) * minEn;
		energies.push_back(e);
		skymaps.push_back(GammaSkymap(nside, e));
	}
}

void GammaSkymapRange::setIntegrator(
    const std::shared_ptr<IntegratorTemplate<QDiffIntensity, QEnergy>>&
        integrator_) {
	for (iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		it->setIntegrator(integrator_);
	}
}

void GammaSkymapRange::setMask(const std::shared_ptr<SkymapMask>& mask_) {
	for (iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		it->setMask(mask_);
	}
}

std::size_t GammaSkymapRange::size() const { return skymaps.size(); }

GammaSkymap GammaSkymapRange::operator[](std::size_t i) const {
	return skymaps[i];
}

void GammaSkymapRange::compute() {
	for (iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		std::cerr << "hermes::SkymapRange: " << it - skymaps.begin() + 1 << "/"
		          << skymaps.size() << ", Energy = " << it->getEnergy() / 1_MeV
		          << " MeV" << std::endl;
		it->compute();
	}
}

void GammaSkymapRange::save(
    const std::shared_ptr<outputs::Output>& output) const {
	output->initOutput();

	for (const_iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		int npix = static_cast<int>((it)->getNpix());

		output->createTable(npix);
		output->writeMetadata(it->getNside(), it->getRes(),
		                      it->getDescription());
		output->writeKeyValueAsString("PIXUNITS", it->getOutputUnitsAsString(),
		                              "Physical units of the skymap pixels");
		output->writeKeyValueAsDouble(std::string("ENERGY"),
		                              static_cast<double>(it->getEnergy()),
		                              std::string("The skymap energy in J."));
		
		auto tempArray = it->containerToRawVector();
		output->writeColumn(npix, tempArray.data());
	}
}

GammaSkymapRange::iterator GammaSkymapRange::begin() { return skymaps.begin(); }

GammaSkymapRange::const_iterator GammaSkymapRange::begin() const {
	return skymaps.begin();
}

GammaSkymapRange::iterator GammaSkymapRange::end() { return skymaps.end(); }

GammaSkymapRange::const_iterator GammaSkymapRange::end() const {
	return skymaps.end();
}

}  // namespace hermes

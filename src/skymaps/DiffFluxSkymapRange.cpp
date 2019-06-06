#include "hermes/skymaps/DiffFluxSkymapRange.h"

#include <iostream>
#include <stdexcept>

namespace hermes {

DiffFluxSkymapRange::DiffFluxSkymapRange(
		std::size_t nside_, QEnergy minEn_,
		QEnergy maxEn_, int enSteps_) :
		nside(nside_), minEn(minEn_),
		maxEn(maxEn_), enSteps(enSteps_) {
	initEnergyRange();
}

DiffFluxSkymapRange::~DiffFluxSkymapRange() {
}

void DiffFluxSkymapRange::initEnergyRange() {
	
	double scaleFactor = std::pow(static_cast<double>(maxEn/minEn),
					1.0/(enSteps-1));

	QEnergy e;
	for (int i = 0; i < enSteps; ++i) {
		e = std::pow(scaleFactor, i) * minEn;
		energies.push_back(e);
		skymaps.push_back(DiffFluxSkymap(nside, e));
	}
}

void DiffFluxSkymapRange::setIntegrator(std::shared_ptr<IntegratorTemplate<QDifferentialFlux, QEnergy> > integrator_) {
	for(iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		it->setIntegrator(integrator_);
	}
}

void DiffFluxSkymapRange::setMask(std::shared_ptr<SkymapMask> mask_) {
	for(iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		it->setMask(mask_);
	}
}

void DiffFluxSkymapRange::compute() {
	for(iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
       		std::cout << "hermes::SkymapRange: "
			  << it - skymaps.begin() + 1 << "/" << skymaps.size()
			  << ", Energy = " << it->getEnergy()/1_MeV << " MeV" << std::endl;
		it->compute();
	}
}

void DiffFluxSkymapRange::save(std::shared_ptr<Output> output) const {
	
	output->initOutput();
	
	for(const_iterator it = skymaps.begin(); it != skymaps.end(); ++it) {
		int npix = static_cast<int>((it)->getNpix());

		output->createTable(npix);
		output->writeMetadata(it->getNside(), it->getRes(), it->getDescription());
		output->writeKeyValueAsDouble(std::string("ENERGY"),
			static_cast<double>(it->getEnergy()),
			std::string("The skymap energy in J."));

		float tempArray[npix];
		for (unsigned long i = 0; i < npix; ++i)
			tempArray[i] = static_cast<float>(it->operator[](i));

		output->writeColumn(npix, tempArray);
	}
}

DiffFluxSkymapRange::iterator DiffFluxSkymapRange::begin() {
        return skymaps.begin();
}

DiffFluxSkymapRange::const_iterator DiffFluxSkymapRange::begin() const {
        return skymaps.begin();
}

DiffFluxSkymapRange::iterator DiffFluxSkymapRange::end() {
        return skymaps.end();
}

DiffFluxSkymapRange::const_iterator DiffFluxSkymapRange::end() const {
        return skymaps.end();
}

} // namespace hermes

#ifndef HERMES_SKYMAPTEMP_H
#define HERMES_SKYMAPTEMP_H

#include "hermes/Units.h"
#include "hermes/ProgressBar.h"
#include "hermes/integrators/Integrator.h"
#include "hermes/skymaps/Skymap.h"
#include "hermes/skymaps/SkymapMask.h"

#if _OPENMP
#include <omp.h>
#define OMP_SCHEDULE static,100
#endif

#include <memory>
#include <vector>
#include <iostream>

namespace hermes {

template <typename Q>
class SkymapTemplate: public Skymap {
protected:
	typedef Q tPixel;
	QDirection iterdir;
	
	typedef std::vector<tPixel> tFluxContainer;
	mutable tFluxContainer fluxContainer;
	mutable std::vector<bool> maskContainer;
	std::shared_ptr<SkymapMask> mask;
	std::shared_ptr<IntegratorTemplate<Q> > integrator;
	
	void initContainer();
	void initMask();
public:
	SkymapTemplate(
		std::size_t nside = 32,
		const std::shared_ptr<SkymapMask> mask_ =
	       		std::make_shared<SkymapMask>(SkymapMask()));
	~SkymapTemplate();
	
	std::size_t getSize() const;
	double operator[](std::size_t ipix) const;
	Q getPixel(std::size_t ipix) const;

	void setIntegrator(std::shared_ptr<IntegratorTemplate<Q> > integrator_);
	void setOutput();
	
	void printPixels();
	virtual void computePixel(
			std::size_t ipix,
			std::shared_ptr<IntegratorTemplate<Q> > integrator_);
	void compute();
	
	/** output **/
	void save(std::shared_ptr<Output> output) const;

        /** iterator goodies */
        typedef typename tFluxContainer::iterator iterator;
        typedef typename tFluxContainer::const_iterator const_iterator;
        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;
};

/* Definitions */

template <typename Q>
void SkymapTemplate<Q>::initContainer() {
	fluxContainer.reserve(npix);
	fluxContainer.insert(fluxContainer.begin(), npix, 0.0);
}

template <typename Q>
void SkymapTemplate<Q>::initMask() {
	maskContainer = mask->getMask(nside);
}

template <typename Q>
SkymapTemplate<Q>::SkymapTemplate(
		std::size_t nside_,
		const std::shared_ptr<SkymapMask> mask_) : Skymap(nside_), mask(mask_) {
	initContainer();
	initMask();
}

template <typename Q>
SkymapTemplate<Q>::~SkymapTemplate() {
	fluxContainer.clear();
}

template <typename Q>
std::size_t SkymapTemplate<Q>::getSize() const {
	return fluxContainer.size();
}

template <typename Q>
Q SkymapTemplate<Q>::getPixel(std::size_t i) const {
        return fluxContainer[i];
}

template <typename Q>
double SkymapTemplate<Q>::operator[](std::size_t i) const {
        return fluxContainer[i].getValue();
}

template <typename Q>
void SkymapTemplate<Q>::printPixels() {
	for (auto i: fluxContainer)
		std::cout << i.getValue() << ' ';
}

template <typename Q>
void SkymapTemplate<Q>::setIntegrator(
		std::shared_ptr<IntegratorTemplate<Q> > integrator_) {
	integrator = integrator_;
}

template <typename Q>
void SkymapTemplate<Q>::computePixel(
		std::size_t ipix,
		std::shared_ptr<IntegratorTemplate<Q> > integrator_) {
	if (maskContainer[ipix] == true) {
		iterdir = pix2ang_ring(getNside(), ipix);
		fluxContainer[ipix] = integrator_->integrateOverLOS(iterdir);
	} else {
		fluxContainer[ipix] = UNSEEN;
	}
}

template <typename Q>
void SkymapTemplate<Q>::compute() {
#if _OPENMP
       	std::cout << "hermes::Integrator: Number of Threads: " << omp_get_max_threads() << std::endl;
#endif
	if(integrator == nullptr)
		throw std::runtime_error("Provide an integrator with the setIntegrator() method");
	
	ProgressBar progressbar(getSize());
	progressbar.start("Compute skymap");

#pragma omp parallel for schedule(OMP_SCHEDULE)
	for (std::size_t ipix = 0; ipix < getSize(); ++ipix) {
			computePixel(ipix, integrator);
#pragma omp critical(progressbarUpdate)
		progressbar.update();
	}
}

template <typename Q>
void SkymapTemplate<Q>::save(std::shared_ptr<Output> output) const {
	
	output->initOutput();
	output->createTable(static_cast<int>(npix));
	output->writeMetadata(nside, res, description);

	float tempArray[npix];
	for (unsigned long i = 0; i < npix; ++i)
		tempArray[i] = static_cast<float>(fluxContainer[i].getValue());

	output->writeColumn(npix, tempArray);
}

template <typename Q>
typename SkymapTemplate<Q>::iterator SkymapTemplate<Q>::begin() {
        return fluxContainer.begin();
}

template <typename Q>
typename SkymapTemplate<Q>::const_iterator SkymapTemplate<Q>::begin() const {
        return fluxContainer.begin();
}

template <typename Q>
typename SkymapTemplate<Q>::iterator SkymapTemplate<Q>::end() {
        return fluxContainer.end();
}

template <typename Q>
typename SkymapTemplate<Q>::const_iterator SkymapTemplate<Q>::end() const {
        return fluxContainer.end();
}

} // namespace hermes

#endif // HERMES_SKYMAPTEMP_H

#ifndef HERMES_SKYMAPTEMP_H
#define HERMES_SKYMAPTEMP_H

#include "hermes/Units.h"
#include "hermes/skymaps/Skymap.h"

#include <memory>
#include <vector>
#include <iostream>

namespace hermes {

template <typename Q>
class SkymapTemplate: public Skymap {
public:
	typedef Q tPixel;
private:
	typedef std::vector<tPixel> tFluxContainer;
	mutable tFluxContainer fluxContainer;
public:
	explicit SkymapTemplate(std::size_t nside = 32);
	~SkymapTemplate();
	std::size_t size() const;
	double operator[](const std::size_t i) const;
	void updatePixel(std::size_t i, tPixel value);
	void print();

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
SkymapTemplate<Q>::SkymapTemplate(std::size_t nside) {
	setNside(nside);
	fluxContainer.reserve(npix);
	fluxContainer.insert(fluxContainer.begin(), npix, 0.0);
}

template <typename Q>
SkymapTemplate<Q>::~SkymapTemplate() {
	fluxContainer.clear();
}

template <typename Q>
std::size_t SkymapTemplate<Q>::size() const {
	return fluxContainer.size();
}

template <typename Q>
double SkymapTemplate<Q>::operator[](const std::size_t i) const {
        return fluxContainer[i].getValue();
}

template <typename Q>
void SkymapTemplate<Q>::updatePixel(std::size_t i, tPixel value) {
	fluxContainer[i] = value;
}

template <typename Q>
void SkymapTemplate<Q>::print() {
	for (auto i: fluxContainer)
		std::cout << i.getValue() << ' ';
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

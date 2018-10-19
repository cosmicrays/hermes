#ifndef HERMES_SKYMAP_H
#define HERMES_SKYMAP_H

#include <memory>
#include <vector>

namespace hermes {

class Skymap {
private:
	typedef std::vector<double> tFluxContainer;
	mutable tFluxContainer fluxContainer;
	unsigned long nside;
	unsigned long npix;

public:
	std::vector<double> map;
	
	/*
 	 *	res	: The resolution index
	 * 	nside	= 2^res		: The number of pixels per side.
	 *	npix	= 12 * nside^2	: The total number of pixels in the map.
	 */
	Skymap(int nside = 32);
	unsigned long getNside();

        /** iterator goodies */
        typedef tFluxContainer::iterator iterator;
        typedef tFluxContainer::const_iterator const_iterator;
        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;
};

} // namespace hermes

#endif // HERMES_SKYMAP_H

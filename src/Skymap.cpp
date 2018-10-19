#include "hermes/Skymap.h"

namespace hermes {

Skymap::Skymap(int nside) : nside(nside) {
	npix = 12 * nside * nside;
	fluxContainer.reserve(npix);
	fluxContainer.insert(fluxContainer.begin(), npix, 0.0);
}

unsigned long Skymap::getNside() {
	return nside;
}

Skymap::iterator Skymap::begin() {
        return fluxContainer.begin();
}

Skymap::const_iterator Skymap::begin() const {
        return fluxContainer.begin();
}

Skymap::iterator Skymap::end() {
        return fluxContainer.end();
}

Skymap::const_iterator Skymap::end() const {
        return fluxContainer.end();
}

} // namespace hermes 

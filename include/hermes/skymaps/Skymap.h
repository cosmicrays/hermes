#ifndef HERMES_SKYMAP_H
#define HERMES_SKYMAP_H

//#include "hermes/outputs/Output.h"

#include <memory>
#include <vector>
#include <iostream>

namespace hermes {

class Skymap {
protected:
	std::size_t nside;
	std::size_t npix;
	std::size_t res;
public:
	/*
 	 *	res	: The resolution index
	 * 	nside	= 2^res		: The number of pixels per side.
	 *	npix	= 12 * nside^2	: The total number of pixels in the map.
	 */
	inline Skymap() { };
	inline ~Skymap() { };
	inline std::size_t log2(std::size_t x) {
		res = -1;
		while (x >>= 1) res++;
		return res;
	}
	inline void setNside(std::size_t n) {
		nside = n;
		res = log2(nside);
		npix = 12 * nside * nside;
	}
	inline std::size_t getNside() {
		return nside;
	}
	inline void setRes(std::size_t r) {
		res = r;
		setNside(1 << res); // 2*res
	}
	inline std::size_t getRes() {
		return res;
	}
	inline std::size_t getNpix() {
		return npix;
	}

	virtual double operator[](const std::size_t i) const = 0;
	//inline void save(std::shared_ptr<Output> output) { output->save(this); }
};

} // namespace hermes

#endif // HERMES_SKYMAP_H

#include "hermes/skymaps/Skymap.h"

#include <typeinfo>

namespace hermes {

Skymap::Skymap(std::size_t nside_) {
	setNside(nside_);
	const std::type_info &info = typeid(*this);
	setDescription(info.name());
}

std::string Skymap::getDescription() const { return description; }

void Skymap::setDescription(const std::string &description_) {
	description = description_;
}

void Skymap::setNside(std::size_t nside_) {
	nside = nside_;
	res = log2(nside_);
	npix = nside2npix(nside_);
}

std::size_t Skymap::getNside() const { return nside; }

void Skymap::setRes(std::size_t res_) {
	res = res_;
	setNside(1 << res); // 2*res
}

std::size_t Skymap::getRes() const { return res; }

std::size_t Skymap::getNpix() const { return npix; }

} // namespace hermes

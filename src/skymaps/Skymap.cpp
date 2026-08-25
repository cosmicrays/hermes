#include "hermes/skymaps/Skymap.h"

#include <limits>
#include <stdexcept>
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
	if (nside_ == 0 || (nside_ & (nside_ - 1)) != 0)
		throw std::invalid_argument("Skymap nside must be a positive power of two");
	const std::size_t max = std::numeric_limits<std::size_t>::max();
	if (nside_ > max / nside_ || nside_ * nside_ > max / 12)
		throw std::out_of_range("Skymap nside is too large");

	nside = nside_;
	res = 0;
	for (std::size_t value = nside_; value > 1; value >>= 1) ++res;
	npix = nside2npix(nside_);
}

std::size_t Skymap::getNside() const { return nside; }

void Skymap::setRes(std::size_t res_) {
	if (res_ >= std::numeric_limits<std::size_t>::digits)
		throw std::out_of_range("Skymap resolution is too large");
	setNside(std::size_t(1) << res_);
}

std::size_t Skymap::getRes() const { return res; }

std::size_t Skymap::getNpix() const { return npix; }

}  // namespace hermes

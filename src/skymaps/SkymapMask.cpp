#include "hermes/skymaps/SkymapMask.h"

namespace hermes {

SkymapMask::SkymapMask() {
	description = "SkymapMask";
}

std::string SkymapMask::getDescription() const {
        return description;
}

void SkymapMask::setDescription(const std::string &description_) {
        description = description_;
}

std::vector<bool> SkymapMask::getMask(std::size_t nside) {
	long npix = nside2npix(nside);
	std::vector<bool> maskContainer(npix, false);

	for (long ipix = 0; ipix < npix; ++ipix) {
		QDirection thetaphi = pix2ang_ring(nside, ipix);
		if(isAllowed(thetaphi)) 
			maskContainer[ipix] = true;
	}

	return maskContainer;
}

RectangularWindow::RectangularWindow(const QAngle &b_top_, const QAngle &b_bottom_,
			const QAngle &l_left_, const QAngle &l_right_) {
      	theta_open  = 90_deg - b_top_;
      	theta_close = 90_deg - b_bottom_;
	phi_open = l_left_;
	phi_close = l_right_;
}

bool RectangularWindow::isAllowed(const QDirection &dir_) {
	if (dir_[0] > theta_open && dir_[0] < theta_close)
		if(dir_[1] > phi_open && dir_[1] < phi_close) 
			return true;
	return false;
}	


} // namespace hermes

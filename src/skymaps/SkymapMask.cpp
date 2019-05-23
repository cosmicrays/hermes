#include "hermes/skymaps/SkymapMask.h"

#include <iostream>

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
	
	phi_open = normalizeAngle(phi_open);
	phi_close = normalizeAngle(phi_close);
}

QAngle RectangularWindow::normalizeAngle(QAngle angle) {
	while (angle < -180_deg) angle += 360_deg;
	while (angle >  180_deg) angle -= 360_deg;
	return angle;
}

bool RectangularWindow::isAngleBetween(
		const QAngle &testAngle, QAngle first, QAngle last) {
	first -= testAngle;
	last  -= testAngle;
	first = normalizeAngle(first);
	last  = normalizeAngle(last);
	if (static_cast<double>(first) * static_cast<double>(last) >= 0)
		return false;
	return fabs(first - last) < 180_deg;
}



bool RectangularWindow::isAllowed(const QDirection &dir_) {
	if (isAngleBetween(dir_[0], theta_open, theta_close))
		if(isAngleBetween(dir_[1], phi_open, phi_close))
			return true;
	return false;
}	


} // namespace hermes

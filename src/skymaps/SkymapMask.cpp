#include "hermes/skymaps/SkymapMask.h"
#include "hermes/Vector3.h"

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

QAngle SkymapMask::normalizeAngle(QAngle angle) const {
	while (angle < -180_deg) angle += 360_deg;
	while (angle >  180_deg) angle -= 360_deg;
	return angle;
}

std::vector<bool> SkymapMask::getMask(std::size_t nside) {
	// TODO: fix not to calculate the mask for every skymap in a skymap range
	long npix = nside2npix(nside);
	std::vector<bool> maskContainer(npix, false);

	for (long ipix = 0; ipix < npix; ++ipix) {
		QDirection thetaphi = pix2ang_ring(nside, ipix);
		if(isAllowed(thetaphi)) 
			maskContainer[ipix] = true;
	}

	return maskContainer;
}

/* CircularWindows class */
InvertMask::InvertMask(const std::shared_ptr<SkymapMask> mask_) : mask(mask_) { 
}

bool InvertMask::isAllowed(const QDirection &dir) const {
	return !mask->isAllowed(dir);
}

/* MaskList class */
MaskList::MaskList() { }

void MaskList::addMask(const std::shared_ptr<SkymapMask> mask_) {
	list.push_back(mask_);
}

bool MaskList::isAllowed(const QDirection &dir) const {
	for(auto &m: list) {
		if (!m->isAllowed(dir))
			return false;	
	}
	return true;
}


/* RectangularWindows class */
RectangularWindow::RectangularWindow(const QAngle &b_top_, const QAngle &b_bottom_,
			const QAngle &l_left_, const QAngle &l_right_) {
      	theta_open  = 90_deg - b_top_;
      	theta_close = 90_deg - b_bottom_;
	phi_open = l_left_;
	phi_close = l_right_;
	
	phi_open = normalizeAngle(phi_open);
	phi_close = normalizeAngle(phi_close);
}

bool RectangularWindow::isAngleBetween(
		const QAngle &testAngle, QAngle first, QAngle last) const {
	first -= testAngle;
	last  -= testAngle;
	first = normalizeAngle(first);
	last  = normalizeAngle(last);
	if (static_cast<double>(first) * static_cast<double>(last) >= 0)
		return false;
	return fabs(first - last) < 180_deg;
}

bool RectangularWindow::isAllowed(const QDirection &dir_) const {
	if (isAngleBetween(dir_[0], theta_open, theta_close))
		if(isAngleBetween(dir_[1], phi_open, phi_close))
			return true;
	return false;
}	

/* CircularWindows class */
CircularWindow::CircularWindow(const QDirection &centre_, const QAngle &aperature_) :
		centre(centre_), aperature(aperature_) {
	centre[0] = normalizeAngle(centre[0]);
	centre[1] = normalizeAngle(centre[1]);
}

bool CircularWindow::isAllowed(const QDirection &dir) const {
	Vector3d v1, v2;
	v1.setRThetaPhi(1, dir[0], dir[1]);
	v2.setRThetaPhi(1, centre[0], centre[1]);
	if (v1.getAngleTo(v2) <= aperature)
			return true;
	return false;
}

} // namespace hermes

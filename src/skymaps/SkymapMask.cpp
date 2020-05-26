#include "hermes/skymaps/SkymapMask.h"

#include <algorithm>

#include "hermes/Common.h"

namespace hermes {

SkymapMask::SkymapMask() : description("SkymapMask") {}

std::string SkymapMask::getDescription() const { return description; }

void SkymapMask::setDescription(const std::string &description_) {
	description = description_;
}

QAngle SkymapMask::normalizeAngle(QAngle angle) const {
	while (angle < -180_deg) angle += 360_deg;
	while (angle > 180_deg) angle -= 360_deg;
	return angle;
}

std::vector<bool> SkymapMask::getMask(std::size_t nside) {
	// TODO(adundovi): fix not to calculate the mask for every skymap in a
	// skymap range
	long npix = nside2npix(nside);
	std::vector<bool> maskContainer(npix, false);

	for (long ipix = 0; ipix < npix; ++ipix) {
		QDirection thetaphi = pix2ang_ring(nside, ipix);
		if (isAllowed(thetaphi)) maskContainer[ipix] = true;
	}

	return maskContainer;
}

/* InvertWindows class */
InvertMask::InvertMask(const std::shared_ptr<SkymapMask> &mask_)
    : mask(mask_) {}

bool InvertMask::isAllowed(const QDirection &dir) const {
	return !mask->isAllowed(dir);
}

/* MaskList class */
MaskList::MaskList() {}

void MaskList::addMask(const std::shared_ptr<SkymapMask> &mask_) {
	list.push_back(mask_);
}

bool MaskList::isAllowed(const QDirection &dir) const {
	// TODO(adundovi): replace with std::none_of
	if (std::all_of(list.cbegin(), list.cend(),
	                [dir](const std::shared_ptr<SkymapMask> &m) {
		                return m->isAllowed(dir);
	                })) {
		return true;
	}
	return false;
}

/* RectangularWindows class */
RectangularWindow::RectangularWindow(const std::array<QAngle, 2> &latitude,
                                     const std::array<QAngle, 2> &longitude)
    : topleft(fromGalCoord({latitude[0], longitude[0]})),
      bottomright(fromGalCoord({latitude[1], longitude[1]})) {}

bool RectangularWindow::isAngleBetween(QAngle testAngle, QAngle open,
                                       QAngle close) const {
	// std::cerr << "open = " << radian2degree(open) << " angle = " <<
	// radian2degree(testAngle) << " close = " << radian2degree(close);
	// If the opening and closing angle are the same, the full circle is allowed
	// since the mask otherwise wouldn't make sanse as it will always return
	// false
	if (open == close) return true;

	// Adopted from: https://stackoverflow.com/a/11412077
	// normalized angle which is less than 180 degrees
	QAngle normAngle =
	    fmod(fmod(close - open, 2 * pi) + 2 * pi * 1_rad, 2 * pi);
	if (normAngle >= pi * 1_rad) {  // swap
		QAngle tmp = open;
		open = close;
		close = tmp;
	}

	if (open <= close) return testAngle >= open && testAngle <= close;
	return testAngle >= open || testAngle <= close;
}

bool RectangularWindow::isAllowed(const QDirection &dir_) const {
	if (isAngleBetween(dir_[0], topleft[0], bottomright[0])) {
		if (isAngleBetween(dir_[1], topleft[1], bottomright[1])) return true;
	}
	return false;
}

/* CircularWindows class */
CircularWindow::CircularWindow(const QDirection &centre_,
                               const QAngle &aperature_)
    : centre(centre_), aperature(aperature_) {
	centre = fromGalCoord(centre);
	v_centre.setRThetaPhi(1, centre[0], centre[1]);
}

bool CircularWindow::isAllowed(const QDirection &dir) const {
	Vector3d v;
	v.setRThetaPhi(1, dir[0], dir[1]);
	if (v.getAngleTo(v_centre) <= aperature) return true;
	return false;
}

}  // namespace hermes

#include "hermes/skymaps/SkymapMask.h"
#include "hermes/Common.h"

#include <algorithm>

namespace hermes {

SkymapMask::SkymapMask() : description("SkymapMask") {}

std::string SkymapMask::getDescription() const { return description; }

void SkymapMask::setDescription(const std::string &description_) {
    description = description_;
}

QAngle SkymapMask::normalizeAngle(QAngle angle) const {
    while (angle < -180_deg)
	angle += 360_deg;
    while (angle > 180_deg)
	angle -= 360_deg;
    return angle;
}

std::vector<bool> SkymapMask::getMask(std::size_t nside) {
    // TODO(adundovi): fix not to calculate the mask for every skymap in a
    // skymap range
    long npix = nside2npix(nside);
    std::vector<bool> maskContainer(npix, false);

    for (long ipix = 0; ipix < npix; ++ipix) {
	QDirection thetaphi = pix2ang_ring(nside, ipix);
	if (isAllowed(thetaphi))
	    maskContainer[ipix] = true;
    }

    return maskContainer;
}

/* InvertWindows class */
InvertMask::InvertMask(const std::shared_ptr<SkymapMask> &mask_) : mask(mask_) {}

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
RectangularWindow::RectangularWindow(const QDirection &topleft_,
				     const QDirection &bottomright_)
    : topleft(fromGalCoord(topleft_)), bottomright(fromGalCoord(bottomright_)) {
}

bool RectangularWindow::isAngleBetween(const QAngle &testAngle, const QAngle &first,
				       const QAngle &last) const {
    /*	first -= testAngle;
	    last  -= testAngle;
	    first = normalizeAngle(first);
	    last  = normalizeAngle(last);
	    if (static_cast<double>(first) * static_cast<double>(last) >= 0)
		    return false;
	    return fabs(first - last) < 180_deg;*/

    if (first == last)
	return true;
    if (testAngle > first && testAngle < last)
	return true;
    return false;
}

bool RectangularWindow::isAllowed(const QDirection &dir_) const {
    if (isAngleBetween(dir_[0], topleft[0], bottomright[0])) {
	if (isAngleBetween(dir_[1], topleft[1], bottomright[1]))
	    return true;
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
    if (v.getAngleTo(v_centre) <= aperature)
	return true;
    return false;
}

} // namespace hermes

#ifndef HERMES_SKYMAPMASK_H
#define HERMES_SKYMAPMASK_H

#include "hermes/HEALPixBits.h"

#include <memory>
#include <vector>

namespace hermes {

class SkymapMask {
private:
	std::string description;
public:
	SkymapMask();
	std::vector<bool> getMask(std::size_t nside);
	virtual bool isAllowed(const QDirection &dir) { return true; }
        
	virtual std::string getDescription() const;
        void setDescription(const std::string &description);
};

class RectangularWindow: public SkymapMask {
private:
	QAngle theta_open, theta_close, phi_open, phi_close;
	QAngle normalizeAngle(QAngle angle);
	bool isAngleBetween(const QAngle &testAngle, QAngle first, QAngle last);
public:
	// in galactic coordinates: b=(-90_deg, 90_deg), l=(-180_deg, 180_deg)
	RectangularWindow(const QAngle &b_top_, const QAngle &b_bottom_,
			const QAngle &l_left_, const QAngle &l_right_);
	bool isAllowed(const QDirection &dir_);
};

} // namespace hermes

#endif // HERMES_SKYMAPMASK_H

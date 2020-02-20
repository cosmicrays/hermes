#ifndef HERMES_SKYMAPMASK_H
#define HERMES_SKYMAPMASK_H

#include "hermes/HEALPixBits.h"

#include <memory>
#include <vector>

namespace hermes {

class SkymapMask {
private:
	std::string description;
protected:
	QAngle normalizeAngle(QAngle angle) const;
public:
	SkymapMask();
	std::vector<bool> getMask(std::size_t nside);
	virtual bool isAllowed(const QDirection &dir) const { return true; }
        
	virtual std::string getDescription() const;
        void setDescription(const std::string &description);
};

class RectangularWindow: public SkymapMask {
private:
	QAngle theta_open, theta_close, phi_open, phi_close;
	bool isAngleBetween(const QAngle &testAngle, QAngle first, QAngle last) const;
public:
	// in galactic coordinates: b=(-90_deg, 90_deg), l=(-180_deg, 180_deg)
	RectangularWindow(const QAngle &b_top_, const QAngle &b_bottom_,
			const QAngle &l_left_, const QAngle &l_right_);
	bool isAllowed(const QDirection &dir_) const;
};

class CircularWindow: public SkymapMask {
private:
	QDirection centre;
	QAngle aperature;
public:
	CircularWindow(const QDirection &centre, const QAngle &aperature);
	bool isAllowed(const QDirection &dir) const;
};

} // namespace hermes

#endif // HERMES_SKYMAPMASK_H

#ifndef HERMES_SKYMAPMASK_H
#define HERMES_SKYMAPMASK_H

#include "hermes/HEALPixBits.h"
#include "hermes/Vector3.h"

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

class InvertMask: public SkymapMask {
private:
	std::shared_ptr<SkymapMask> mask;
public:
	InvertMask(const std::shared_ptr<SkymapMask> mask);
	bool isAllowed(const QDirection &dir) const override;
};

class MaskList: public SkymapMask {
private:
	std::vector<std::shared_ptr<SkymapMask>> list;
public:
	MaskList();
	void addMask(const std::shared_ptr<SkymapMask> mask);
	bool isAllowed(const QDirection &dir) const override;
};


class RectangularWindow: public SkymapMask {
private:
	QDirection topleft, bottomright;
	bool isAngleBetween(const QAngle &testAngle, QAngle first, QAngle last) const;
public:
	// in galactic coordinates: b=(-90_deg, 90_deg), l=(-180_deg, 180_deg),
	// top - bottom = lattitude
	// left - right = longitude
	RectangularWindow(const QDirection &topleft,
			  const QDirection &bottomright);
	bool isAllowed(const QDirection &dir_) const override;
};

class CircularWindow: public SkymapMask {
private:
	QDirection centre;
	Vector3d v_centre;
	QAngle aperature;
public:
	CircularWindow(const QDirection &centre, const QAngle &aperature);
	bool isAllowed(const QDirection &dir) const override;
};

} // namespace hermes

#endif // HERMES_SKYMAPMASK_H

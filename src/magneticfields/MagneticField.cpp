#include "hermes/magneticfields/MagneticField.h"

namespace hermes {
namespace magneticfields {

PeriodicMagneticField::PeriodicMagneticField(
    std::shared_ptr<MagneticField> field, const Vector3QLength &extends)
    : field(field), extends(extends), origin(0_kpc), reflective(false) {}

PeriodicMagneticField::PeriodicMagneticField(
    std::shared_ptr<MagneticField> field, const Vector3QLength &extends,
    const Vector3QLength &origin, bool reflective)
    : field(field), extends(extends), origin(origin), reflective(reflective) {}

Vector3QLength &PeriodicMagneticField::getOrigin() { return origin; }

void PeriodicMagneticField::setOrigin(const Vector3QLength &origin) {
    this->origin = origin;
}

Vector3QLength &PeriodicMagneticField::getExtends() { return extends; }

void PeriodicMagneticField::setExtends(const Vector3QLength &extends) {
    this->extends = extends;
}

bool PeriodicMagneticField::isReflective() { return reflective; }

void PeriodicMagneticField::setReflective(bool reflective) {
    this->reflective = reflective;
}

Vector3QMField
PeriodicMagneticField::getField(const Vector3QLength &position) const {
    Vector3d n =
	(((position - origin).getValue() / extends.getValue()).floor());
    Vector3QLength p = position - origin - n * extends;

    if (reflective) {
	long mx = static_cast<long>(std::fabs(n.x)) % 2;
	if (mx == 1)
	    p.x = extends.x - p.x;
	long my = static_cast<long>(std::fabs(n.y)) % 2;
	if (my == 1)
	    p.y = extends.y - p.y;
	long mz = static_cast<long>(std::fabs(n.z)) % 2;
	if (mz == 1)
	    p.z = extends.z - p.z;
    }

    return field->getField(p);
}

void MagneticFieldList::addField(std::shared_ptr<MagneticField> field) {
    fields.push_back(field);
}

Vector3QMField
MagneticFieldList::getField(const Vector3QLength &position) const {
    Vector3QMField b;
    for (int i = 0; i < fields.size(); i++)
	b += fields[i]->getField(position);
    return b;
}

} // namespace magneticfields
} // namespace hermes

#include "hermes/magneticField/MagneticField.h"

namespace hermes {

PeriodicMagneticField::PeriodicMagneticField(std::shared_ptr<MagneticField> field,
		const Vector3Length &extends) :
		field(field), extends(extends), origin(0_kpc), reflective(false) {

}

PeriodicMagneticField::PeriodicMagneticField(std::shared_ptr<MagneticField> field,
		const Vector3Length &extends, const Vector3Length &origin, bool reflective) :
		field(field), extends(extends), origin(origin), reflective(reflective) {

}

Vector3Length &PeriodicMagneticField::getOrigin() {
	return origin;
}

void PeriodicMagneticField::setOrigin(const Vector3Length &origin) {
	this->origin = origin;
}

Vector3Length &PeriodicMagneticField::getExtends() {
	return extends;
}

void PeriodicMagneticField::setExtends(const Vector3Length &origin) {
	this->extends = extends;
}

bool PeriodicMagneticField::isReflective() {
	return reflective;
}

void PeriodicMagneticField::setReflective(bool reflective) {
	this->reflective = reflective;
}

Vector3MField PeriodicMagneticField::getField(const Vector3Length &position) const {
	Vector3d n = (((position - origin) / extends).floor());
	Vector3Length p = position - origin - n * extends;

	if (reflective) {
		long mx = (long) std::fabs(n.x) % 2;
		if (mx == 1)
			p.x = extends.x - p.x;
		long my = (long) std::fabs(n.y) % 2;
		if (my == 1)
			p.y = extends.y - p.y;
		long mz = (long) std::fabs(n.z) % 2;
		if (mz == 1)
			p.z = extends.z - p.z;
	}

	return field->getField(p);
}

void MagneticFieldList::addField(std::shared_ptr<MagneticField> field) {
	fields.push_back(field);
}

Vector3MField MagneticFieldList::getField(const Vector3Length &position) const {
	Vector3MField b;
	for (int i = 0; i < fields.size(); i++)
		b += fields[i]->getField(position);
	return b;
}

} // namespace hermes

#ifndef HERMES_MAGNETICFIELD_H
#define HERMES_MAGNETICFIELD_H

#include "hermes/Vector3.h"
#include "hermes/Vector3Quantity.h"
#include "hermes/Units.h"
#include <memory>

#ifdef HERMES_HAVE_MUPARSER
#include "muParser.h"
#endif

namespace hermes {
/**
 * \addtogroup MagneticFields
 * @{
 */

/**
 @class MagneticField
 @brief Abstract base class for magnetic fields.
 */
class MagneticField {
public:
	virtual ~MagneticField() {
	}
	virtual Vector3MField getField(const Vector3Length &position) const {
		return Vector3MField(0_muG);
	};
};

/**
 @class PeriodicMagneticField
 @brief Magnetic field decorator implementing periodic fields.
 */
class PeriodicMagneticField: public MagneticField {
	std::shared_ptr<MagneticField> field;
	Vector3Length origin, extends;
	bool reflective;
public:
	PeriodicMagneticField(std::shared_ptr<MagneticField> field,
			const Vector3Length &extends);
	PeriodicMagneticField(std::shared_ptr<MagneticField> field, const Vector3Length &extends,
			const Vector3Length &origin, bool reflective);
	Vector3Length &getOrigin();
	void setOrigin(const Vector3Length &origin);
	Vector3Length &getExtends();
	void setExtends(const Vector3Length &origin);
	bool isReflective();
	void setReflective(bool reflective);
	Vector3MField getField(const Vector3Length &position) const;
};

/**
 @class MagneticFieldList
 @brief Magnetic field decorator implementing a superposition of fields.
 */
class MagneticFieldList: public MagneticField {
	std::vector<std::shared_ptr<MagneticField> > fields;
public:
	void addField(std::shared_ptr<MagneticField> field);
	Vector3MField getField(const Vector3Length &position) const;
};

/**
 @class UniformMagneticField
 @brief Magnetic field with one B-field vector.
 */
class UniformMagneticField: public MagneticField {
	Vector3MField value;
public:
	UniformMagneticField(const Vector3MField &value) :
			value(value) {
	}
	Vector3MField getField(const Vector3Length &position) const {
		return value;
	}
};

/** @} */
} // namespace hermes

#endif // HERMES_MAGNETICFIELD_H

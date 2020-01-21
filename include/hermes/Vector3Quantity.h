#ifndef HERMES_VECTOR3QUANTITY_H
#define HERMES_VECTOR3QUANTITY_H

#include <iostream>
#include <cmath>
#include <vector>
#include <limits>

#include "Vector3.h"
#include "hermes/Units.h"

namespace hermes {

/**
 * \addtogroup Core
 * @{
 */

/**
 @class Vector3Qunatity
 @brief Template class for 3-vectors of type Quantity.

 Allows accessing and changing the elements x, y, z directly or  through the
 corresponding get and set methods.

 Angle definitions are
 phi [-pi, pi]: azimuthal angle in the x-y plane, 0 pointing in x-direction
 theta [0, pi]: zenith angle towards the z axis, 0 pointing in z-direction
 */
template<typename T>
class Vector3Quantity: public Vector3<T> {
public:
	Vector3Quantity() : Vector3<T>() {
	}

	// Provides implicit conversion
	Vector3Quantity(const Vector3Quantity<T> &v) :
			Vector3<T>(v.getX(), v.getY(), v.getZ()) {
	}
	
	template<typename U>
	Vector3Quantity(const Vector3Quantity<U> &v) :
			Vector3<T>(static_cast<T>(v.getX()),
				   static_cast<T>(v.getY()),
				   static_cast<T>(v.getZ())) {
	}
	
	template<typename U>
	Vector3Quantity(const Vector3<U> &v) :
			Vector3<T>(static_cast<T>(v.getX()),
				   static_cast<T>(v.getY()),
				   static_cast<T>(v.getZ())) {
	}

	explicit Vector3Quantity(const T &X) :
			Vector3<T>(X, X, X) {
	}
	
	explicit Vector3Quantity(const T &X, const T &Y, const T &Z) :
			Vector3<T>(X, Y, Z) {
	}
/*	
	explicit Vector3Quantity(const int *v) :
			x(v[0]), y(v[1]), z(v[2]) {
	}

	explicit Vector3Quantity(const double *v) :
			x(v[0]), y(v[1]), z(v[2]) {
	}

	explicit Vector3Quantity(const float *v) :
			x(v[0]), y(v[1]), z(v[2]) {
	}
	
	explicit Vector3Quantity(T t) :
			Vector3d(t.getValue()), x(t), y(t), z(t) {
	}
*/
	Vector3d getValue() const {
		return Vector3d(static_cast<double>(this->x),
				static_cast<double>(this->y),
				static_cast<double>(this->z));
	}
	
	// return the azimuth angle
	QAngle getPhi() const {
		T eps = std::numeric_limits < T > ::min();
		if ((fabs(this->x) < eps) and (fabs(this->y) < eps))
			return 0.0;
		else
			return atan2(this->y, this->x);
	}

	// return the zenith angle
	QAngle getTheta() const {
		T eps = std::numeric_limits < T > ::min();
		if ((fabs(this->x) < eps) and (fabs(this->y) < eps) and (fabs(this->z) < eps))
			return 0.0;
		else
			return atan2((T) sqrt(this->x * this->x + this->y * this->y), this->z);
	}

	// return the unit-vector e_theta
	Vector3Quantity<T> getUnitVectorTheta() const {
		T theta = this->getTheta();
		T phi = this->getPhi();
		return Vector3Quantity<T>(cos(theta) * cos(phi), cos(theta) * sin(phi),
				-sin(theta));
	}

	// return the unit-vector e_phi
	Vector3Quantity<T> getUnitVectorPhi() const {
		return Vector3Quantity<T>(-sin(this->getPhi()), cos(this->getPhi()), 0);
	}

	// return the angle [0, pi] between the vectors
	QAngle getAngleTo(const Vector3Quantity<T> &v) const {
		double cosdistance = static_cast<double>(this->dot(v) / v.getR() / this->getR());
		// In some directions cosdistance is > 1 on some compilers
		// This ensures that the correct result is returned
		if (cosdistance >= 1.)
			return QAngle(0);
		else if (cosdistance <= -1.)
			return QAngle(M_PI);
		else
			return acos(cosdistance);
	}

	// rotate the vector around a given axis by a given a angle
	Vector3Quantity<T> getRotated(const Vector3Quantity<T> &axis, T angle) const {
		Vector3Quantity<T> u = axis; 
		T c = cos(angle);
		T s = sin(angle);
		Vector3Quantity<T> Rx(c + u.x * u.x * (1 - c), u.x * u.y * (1 - c) - u.z * s,
				u.x * u.z * (1 - c) + u.y * s);
		Vector3Quantity<T> Ry(u.y * u.x * (1 - c) + u.z * s, c + u.y * u.y * (1 - c),
				u.y * u.z * (1 - c) - u.x * s);
		Vector3Quantity<T> Rz(u.z * u.x * (1 - c) - u.y * s,
				u.z * u.y * (1 - c) + u.x * s, c + u.z * u.z * (1 - c));
		return Vector3Quantity<T>(dot(Rx), dot(Ry), dot(Rz));
	}

	// return vector with values limited to the range [lower, upper]
	Vector3Quantity<T> clip(T lower, T upper) const {
		Vector3Quantity<T> out;
		out.x = std::max(lower, std::min(this->x, upper));
		out.y = std::max(lower, std::min(this->y, upper));
		out.z = std::max(lower, std::min(this->z, upper));
		return out;
	}

	// return vector with absolute values
	Vector3Quantity<T> abs() const {
		return Vector3Quantity<T>(std::abs(this->x), std::abs(this->y), std::abs(this->z));
	}

	// return vector with floored values
	Vector3Quantity<T> floor() const {
		return Vector3Quantity<T>(std::floor(this->x), std::floor(this->y), std::floor(this->z));
	}

	// return vector with ceiled values
	Vector3Quantity<T> ceil() const {
		return Vector3Quantity<T>(std::ceil(this->x), std::ceil(this->y), std::ceil(this->z));
	}

	// dot product
	//auto dot(const Vector3Quantity<T> &v) const {
	auto dot(const Vector3Quantity<T> &v) const {
		return this->x * v.x + this->y * v.y + this->z * v.z;
	}

	// cross product
	Vector3Quantity<T> cross(const Vector3Quantity<T> &v) const {
		return Vector3Quantity<T>(this->y * v.z - v.y * this->z, this->z * v.x - v.z * this->x,
				this->x * v.y - v.x * this->y);
	}

	// returns true if all elements of the two vectors are equal
	bool operator ==(const Vector3Quantity<T> &v) const {
		if (this->x != v.x)
			return false;
		if (this->y != v.y)
			return false;
		if (this->z != v.z)
			return false;
		return true;
	}

	
	Vector3Quantity<T> operator +(const Vector3Quantity<T> &v) const {
		return Vector3Quantity(this->x + v.x, this->y + v.y, this->z + v.z);
	}

	Vector3Quantity<T> operator +(const T &f) const {
		return Vector3Quantity(this->x + f, this->y + f, this->z + f);
	}

	Vector3Quantity<T> operator -(const Vector3Quantity<T> &v) const {
		return Vector3Quantity(this->x - v.x, this->y - v.y, this->z - v.z);
	}

	Vector3Quantity<T> operator -(const T &f) const {
		return Vector3Quantity(this->x - f, this->y - f, this->z - f);
	}
	
	
	// element-wise division
	Vector3Quantity<T> operator /(const Vector3Quantity<T> &v) const {
		return Vector3Quantity<T>(this->x / v.x, this->y / v.y, this->z / v.z);
	}

	Vector3Quantity<T> operator /(const T &f) const {
		return Vector3Quantity<T>(this->x / f, this->y / f, this->z / f);
	}
	
	// element-wise modulo operation
	Vector3Quantity<T> operator %(const Vector3Quantity<T> &v) const {
		return Vector3Quantity<T>(fmod(this->x, v.x), fmod(this->y, v.y), fmod(this->z, v.z));
	}

	Vector3Quantity<T> operator %(const T &f) const {
		return Vector3Quantity<T>(fmod(this->x, f), fmod(this->y, f), fmod(this->z, f));
	}

	Vector3Quantity<T> &operator -=(const Vector3Quantity<T> &v) {
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		return *this;
	}

	Vector3Quantity<T> &operator -=(const T &f) {
		this->x -= f;
		this->y -= f;
		this->z -= f;
		return *this;
	}

	Vector3Quantity<T> &operator +=(const Vector3Quantity<T> &v) {
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		return *this;
	}

	Vector3Quantity<T> &operator +=(const T &f) {
		this->x += f;
		this->y += f;
		this->z += f;
		return *this;
	}

	// element-wise multiplication
	Vector3Quantity<T> &operator *=(const Vector3Quantity<T> &v) {
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		return *this;
	}

	Vector3Quantity<T> &operator *=(const T &f) {
		this->x *= f;
		this->y *= f;
		this->z *= f;
		return *this;
	}

	// element-wise division
	Vector3Quantity<T> &operator /=(const Vector3Quantity<T> &v) {
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;
		return *this;
	}

	Vector3Quantity<T> &operator /=(const T &f) {
		this->x /= f;
		this->y /= f;
		this->z /= f;
		return *this;
	}

	// element-wise modulo operation
	Vector3Quantity<T> &operator %=(const Vector3Quantity<T> &v) {
		this->x = fmod(this->x, v.x);
		this->y = fmod(this->y, v.y);
		this->z = fmod(this->z, v.z);
		return *this;
	}

	Vector3Quantity<T> &operator %=(const T &f) {
		this->x = fmod(this->x, f);
		this->y = fmod(this->y, f);
		this->z = fmod(this->z, f);
		return *this;
	}

	Vector3Quantity<T> &operator =(const Vector3Quantity<T> &v) {
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		return *this;
	}

	Vector3Quantity<T> &operator =(const T &f) {
		this->x = f;
		this->y = f;
		this->z = f;
		return *this;
	}
};

template<typename T, typename Q>
inline Vector3Quantity<Q> operator *(const Vector3Quantity<Q> &q, const Vector3<T> &v) {
	return Vector3Quantity<Q>(v.x * q.x, v.y * q.y, v.z * q.z);
}
template<typename T, typename Q>
inline Vector3Quantity<Q> operator *(const Vector3<T> &v, const Vector3Quantity<Q> &q) {
	return Vector3Quantity<Q>(v.x * q.x, v.y * q.y, v.z * q.z);
}
#ifndef HERMES_UNITSDISABLE
template<typename T, typename Q>
inline Vector3Quantity<Q> operator *(const Vector3<T> &v, const Q q) {
	return Vector3Quantity<Q>(T(v.x * q), T(v.y * q), T(v.z * q));
}
template<typename Q>
inline Vector3Quantity<Q> operator *(const Vector3Quantity<Q> &q, const double a) {
	return Vector3Quantity<Q>(q.x * a, q.y * a, q.z * a);
}
#endif
template<typename Q>
inline Vector3Quantity<Q> operator *(const Vector3Quantity<Q> &q, const QNumber &a) {
	return Vector3Quantity<Q>(q.x * a, q.y * a, q.z * a);
}

typedef Vector3Quantity<QNumber> Vector3QNumber;
typedef Vector3Quantity<QLength> Vector3QLength;
typedef Vector3Quantity<QMField> Vector3QMField;

template<typename T>
inline Vector3QMField operator *(const Vector3<T> &q, const QMField &a) {
	return Vector3QMField(q.x * a, q.y * a, q.z * a);
}

/** @}*/
} // namespace hermes

#endif // HERMES_VECTOR3QUANTITY_H

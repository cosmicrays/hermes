#ifndef HERMES_VECTOR3QUANTITY_H
#define HERMES_VECTOR3QUANTITY_H

#include <iostream>
#include <cmath>
#include <vector>
#include <limits>

#include "Vector3.h"
#include "Units.h"

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
class Vector3Quantity: public Vector3d {
public:
	T x, y, z;

	Vector3Quantity() :
			x(T(0)), y(T(0)), z(T(0)) {
	}

	// Provides implicit conversion
	template<typename U>
	Vector3Quantity(const Vector3Quantity<U> &v) :
			x(v.x), y(v.y), z(v.z) {
	}
	
	explicit Vector3Quantity(const int *v) :
			x(v[0]), y(v[1]), z(v[2]) {
	}

	explicit Vector3Quantity(const double *v) :
			x(v[0]), y(v[1]), z(v[2]) {
	}

	explicit Vector3Quantity(const float *v) :
			x(v[0]), y(v[1]), z(v[2]) {
	}

	explicit Vector3Quantity(const T &X, const T &Y, const T &Z) :
			x(X), y(Y), z(Z) {
	}

	explicit Vector3Quantity(T t) :
			x(t), y(t), z(t) {
	}

	Vector3d getValue() {
		return Vector3d(x.getValue(), y.getValue(), z.getValue());
	}

	void setRThetaPhi(const T r, const QAngle theta, const QAngle phi) {
		x = r * sin(theta) * cos(phi);
		y = r * sin(theta) * sin(phi);
		z = r * cos(theta);
	}
	
	T getX() const {
		return x;
	}

	T getY() const {
		return y;
	}

	T getZ() const {
		return z;
	}

	// magnitude (2-norm) of the vector
	T getR() const {
		return sqrt(x * x + y * y + z * z);
	}

	// return the azimuth angle
	QAngle getPhi() const {
		T eps = std::numeric_limits < T > ::min();
		if ((fabs(x) < eps) and (fabs(y) < eps))
			return 0.0;
		else
			return atan2(y, x);
	}

	// return the zenith angle
	QAngle getTheta() const {
		T eps = std::numeric_limits < T > ::min();
		if ((fabs(x) < eps) and (fabs(y) < eps) and (fabs(z) < eps))
			return 0.0;
		else
			return atan2((T) sqrt(x * x + y * y), z);
	}

	// return the unit-vector e_theta
	Vector3Quantity<T> getUnitVectorTheta() const {
		T theta = getTheta();
		T phi = getPhi();
		return Vector3Quantity<T>(cos(theta) * cos(phi), cos(theta) * sin(phi),
				-sin(theta));
	}

	// return the unit-vector e_phi
	Vector3Quantity<T> getUnitVectorPhi() const {
		return Vector3Quantity<T>(-sin(getPhi()), cos(getPhi()), 0);
	}

	// return the angle [0, pi] between the vectors
	QAngle getAngleTo(const Vector3Quantity<T> &v) const {
		double cosdistance = (dot(v) / v.getR() / getR()).getValue();
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
		out.x = std::max(lower, std::min(x, upper));
		out.y = std::max(lower, std::min(y, upper));
		out.z = std::max(lower, std::min(z, upper));
		return out;
	}

	// return vector with absolute values
	Vector3Quantity<T> abs() const {
		return Vector3Quantity<T>(std::abs(x), std::abs(y), std::abs(z));
	}

	// return vector with floored values
	Vector3Quantity<T> floor() const {
		return Vector3Quantity<T>(std::floor(x), std::floor(y), std::floor(z));
	}

	// return vector with ceiled values
	Vector3Quantity<T> ceil() const {
		return Vector3Quantity<T>(std::ceil(x), std::ceil(y), std::ceil(z));
	}

	// minimum element
	T min() const {
		return std::min(x, std::min(y, z));
	}

	// maximum element
	T max() const {
		return std::max(x, std::max(y, z));
	}
	
	// dot product
	auto dot(const Vector3Quantity<T> &v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	// cross product
	Vector3Quantity<T> cross(const Vector3Quantity<T> &v) const {
		return Vector3Quantity<T>(y * v.z - v.y * z, z * v.x - v.z * x,
				x * v.y - v.x * y);
	}

	// returns true if all elements of the two vectors are equal
	bool operator ==(const Vector3Quantity<T> &v) const {
		if (x != v.x)
			return false;
		if (y != v.y)
			return false;
		if (z != v.z)
			return false;
		return true;
	}

	Vector3Quantity<T> operator +(const Vector3Quantity<T> &v) const {
		return Vector3Quantity(x + v.x, y + v.y, z + v.z);
	}

	Vector3Quantity<T> operator +(const T &f) const {
		return Vector3Quantity(x + f, y + f, z + f);
	}

	Vector3Quantity<T> operator -(const Vector3Quantity<T> &v) const {
		return Vector3Quantity(x - v.x, y - v.y, z - v.z);
	}

	Vector3Quantity<T> operator -(const T &f) const {
		return Vector3Quantity(x - f, y - f, z - f);
	}
	/*
	// element-wise division
	Vector3Quantity<T> operator /(const Vector3Quantity<T> &v) const {
		return Vector3Quantity(x / v.x, y / v.y, z / v.z);
	}

	Vector3Quantity<T> operator /(const T &f) const {
		return Vector3Quantity<T>(x / f, y / f, z / f);
	}
	*/
	// element-wise modulo operation
	Vector3Quantity<T> operator %(const Vector3Quantity<T> &v) const {
		return Vector3(fmod(x, v.x), fmod(y, v.y), fmod(z, v.z));
	}

	Vector3Quantity<T> operator %(const T &f) const {
		return Vector3(fmod(x, f), fmod(y, f), fmod(z, f));
	}

	Vector3Quantity<T> &operator -=(const Vector3Quantity<T> &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3Quantity<T> &operator -=(const T &f) {
		x -= f;
		y -= f;
		z -= f;
		return *this;
	}

	Vector3Quantity<T> &operator +=(const Vector3Quantity<T> &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3Quantity<T> &operator +=(const T &f) {
		x += f;
		y += f;
		z += f;
		return *this;
	}

	// element-wise multiplication
	Vector3Quantity<T> &operator *=(const Vector3Quantity<T> &v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	Vector3Quantity<T> &operator *=(const T &f) {
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	// element-wise division
	Vector3Quantity<T> &operator /=(const Vector3Quantity<T> &v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	Vector3Quantity<T> &operator /=(const T &f) {
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}

	// element-wise modulo operation
	Vector3Quantity<T> &operator %=(const Vector3Quantity<T> &v) {
		x = fmod(x, v.x);
		y = fmod(y, v.y);
		z = fmod(z, v.z);
		return *this;
	}

	Vector3Quantity<T> &operator %=(const T &f) {
		x = fmod(x, f);
		y = fmod(y, f);
		z = fmod(z, f);
		return *this;
	}

	Vector3Quantity<T> &operator =(const Vector3Quantity<T> &v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	Vector3Quantity<T> &operator =(const T &f) {
		x = f;
		y = f;
		z = f;
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
template<typename T, typename Q>
inline Vector3Quantity<Q> operator *(const Vector3<T> &v, const Q q) {
	return Vector3Quantity<Q>(v.x * q, v.y * q, v.z * q);
}
template<typename Q>
inline Vector3Quantity<Q> operator *(const Vector3Quantity<Q> &q, const double a) {
	return Vector3Quantity<Q>(q.x * a, q.y * a, q.z * a);
}

typedef Vector3Quantity<QLength> Vector3QLength;
typedef Vector3Quantity<QMField> Vector3QMField;

/** @}*/
} // namespace hermes

#endif // HERMES_VECTOR3QUANTITY_H

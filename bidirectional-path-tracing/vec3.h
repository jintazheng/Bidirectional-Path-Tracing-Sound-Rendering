#pragma once

#include <math.h>

class Vec3 {

protected:
	float e[3];

public:
	Vec3() {}
	Vec3(float const e0, float const e1, float const e2) { e[0] = e0; e[1] = e1; e[2] = e2; }

	// Accessor and setter methods
	inline float x() const { return e[0]; }
	inline float y() const { return e[1]; }
	inline float z() const { return e[2]; }
	inline float r() const { return e[0]; }
	inline float g() const { return e[1]; }
	inline float b() const { return e[2]; }

	inline void operator=(Vec3 const& v2) { e[0] = v2.e[0]; e[1] = v2.e[1]; e[2] = v2.e[2]; }
	inline float operator[](int const i) const { return e[i]; }
	inline float& operator[](int const i) { return e[i]; };

	// Operations that don't modify the vector
	inline Vec3 operator+(Vec3 const& v2) const { return Vec3(e[0] + v2.e[0], e[1] + v2.e[1], e[2] + v2.e[2]); }
	inline Vec3 operator-(Vec3 const& v2) const { return Vec3(e[0] - v2.e[0], e[1] - v2.e[1], e[2] - v2.e[2]); }
	inline Vec3 operator*(Vec3 const& v2) const { return Vec3(e[0] * v2.e[0], e[1] * v2.e[1], e[2] * v2.e[2]); }
	inline Vec3 operator/(Vec3 const& v2) const { return Vec3(e[0] / v2.e[0], e[1] / v2.e[1], e[2] / v2.e[2]); }
	inline Vec3 operator*(float const t) const { return Vec3(e[0] * t, e[1] * t, e[2] * t); }
	inline Vec3 operator/(float const t) const { return Vec3(e[0] / t, e[1] / t, e[2] / t); }

	// Operations that modify the vector
	inline Vec3& operator+=(Vec3 const& v2) { *this = *this + v2; return *this; }
	inline Vec3& operator-=(Vec3 const& v2) { *this = *this - v2; return *this; }
	inline Vec3& operator*=(Vec3 const& v2) { *this = *this * v2; return *this; }
	inline Vec3& operator/=(Vec3 const& v2) { *this = *this / v2; return *this; }
	inline Vec3& operator*=(float const t) { *this = *this * t; return *this; }
	inline Vec3& operator/=(float const t) { *this = *this / t; return *this; }

	// Utility functions
	inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
	inline float lengthSquared() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
	inline void normalize() { *this /= this->length(); }
	inline Vec3 unitVec() const { return *this / this->length(); }
	inline void clamp() { this->e[0] = fmin(1.f, fmax(0.f, this->e[0])); this->e[1] = fmin(1.f, fmax(0.f, this->e[1])); this->e[2] = fmin(1.f, fmax(0.f, this->e[2])); }
};

// Commutativity operations
inline Vec3 operator*(float const t, Vec3 const& v2) { return Vec3(t * v2[0], t * v2[1], t * v2[2]); }
inline Vec3 operator/(float const t, Vec3 const& v2) { return Vec3(t / v2[0], t / v2[1], t / v2[2]); }

// Utility functions not called on an instance
inline Vec3 normalize(Vec3 const& v1) { return v1 / v1.length(); }
inline float magnitudeSquared(Vec3 const& v1) { return v1.x() * v1.x() + v1.y() * v1.y() + v1.z() * v1.z(); }
inline float dot(Vec3 const& v1, Vec3 const& v2) { return v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z(); }
inline Vec3 cross(Vec3 const& v1, Vec3 const& v2) { return Vec3((v1.y() * v2.z() - v1.z() * v2.y()), -(v1.x() * v2.z() - v1.z() * v2.x()), (v1.x() * v2.y() - v1.y() * v2.x())); }
inline Vec3 proj(Vec3 const& onto, Vec3 const& v2) { return dot(onto, v2) / dot(onto, onto) * onto; }
inline float angle(Vec3 const& first, Vec3 const& second) { return acos(dot(first, second) / sqrt(first.lengthSquared() * second.lengthSquared())); }
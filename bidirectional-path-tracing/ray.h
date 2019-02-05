#pragma once

#include "vec3.h"

class Ray
{
public:
	Ray() {}
	Ray(Vec3 const& a, Vec3 const& b) { 
		A = a; 
		B = b;
		invDir = 1.f / B;
		sign[0] = !signbit(B.x());
		sign[1] = !signbit(B.y());
		sign[2] = !signbit(B.z());
	}
	Vec3 origin() const { return A; }
	Vec3 direction() const { return B; }
	Vec3 negDirection() const { return -1 * B; }
	Vec3 point_at_parameter(float const t) const { return A + B * t; }

	Vec3 A, B;
	Vec3 invDir;
	bool sign[3];
};
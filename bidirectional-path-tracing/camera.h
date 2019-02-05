#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include "util.h"
#include "ray.h"

class Camera {
public:
	Camera(Vec3 lookfrom, Vec3 lookat, Vec3 up, float const vfov, float const aspect, float const aperture, float focus_dist) {
		lens_radius = aperture / 2.f;
		float const theta = vfov * M_PI / 180.f;
		float const halfHeight = tan(theta / 2.f);
		float const halfWidth = aspect * halfHeight;

		origin = lookfrom;
		w = (lookfrom - lookat).unitVec();
		u = cross(up, w).unitVec();
		v = cross(w, u);


		lower_left_corner = origin - (halfWidth * focus_dist * u) - (halfHeight * focus_dist * v) - focus_dist * w;
		horizontal = 2 * halfWidth * focus_dist * u;
		vertical = 2 * halfHeight * focus_dist * v;
	}

	Ray get_ray(float const s, float const t) {
		Vec3 randomDisk = lens_radius * RandInDisk();
		Vec3 offset = u * randomDisk.x() + v * randomDisk.y();
		return Ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
	}

	Vec3 origin;
	Vec3 lower_left_corner;
	Vec3 horizontal;
	Vec3 vertical;
	Vec3 u, v, w;
	float lens_radius;
};
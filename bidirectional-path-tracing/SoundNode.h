#pragma once

#include "object.h"

class SoundNode : public Sphere {
public:

	SoundNode(Vec3 const cen, float const r, Material* m) : Sphere(cen, r, m) {}

	// Sound nodes cannot be intersected
	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const {
		return false;
	}
};
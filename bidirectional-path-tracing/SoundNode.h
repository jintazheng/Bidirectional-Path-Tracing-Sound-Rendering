#pragma once

#include "object.h"

class SoundNode : public Object {
public:
	SoundNode(Vec3 location) : mLocation(location) {}

	// Sound nodes cannot be intersected
	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const {
		return false;
	}

	Vec3 mLocation;
};
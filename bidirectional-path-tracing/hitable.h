#pragma once

#include "ray.h"
#include "Box.h"

class Material;

struct HitRecord {
	float t;
	Vec3 p;
	Vec3 normal;
	Material* material;
};

class Hitable {
public:
	virtual bool Hit(Ray const& r, float t_min, float t_max, HitRecord& rec) const = 0;
	Box bounds;
};
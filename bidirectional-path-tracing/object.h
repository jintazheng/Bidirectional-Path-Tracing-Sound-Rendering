#pragma once

#include "ray.h"
#include "Box.h"

class Material;

struct HitRecord {
	float t;
	Vec3 p;
	Vec3 barycentric;
	Vec3 normal;
	Material* material;
};

class Object {
public:
	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const = 0;
	virtual bool HitBB(Ray const& r) const {
		return mBoundingBox.Hit(r);
	}

	virtual void Translate(Vec3 const& trans) {
		mBoundingBox.Translate(trans);
	}

	virtual void Draw(sf::Shader* shader) = 0;

	Box mBoundingBox;
};
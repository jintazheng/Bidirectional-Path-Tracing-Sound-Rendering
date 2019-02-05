#pragma once

#include "util.h"

class Light : public Object {
public:
	virtual Vec3 RandInLight() = 0;
	float mIntensity;
};

class SphereLight : public Light {
public: 
	SphereLight(Vec3 const& pos, Vec3 const& size, float const intensity) {
		mIntensity = intensity;
		mBoundingBox.Expand(pos + size / 2.f);
		mBoundingBox.Expand(pos - size / 2.f);
		mSphere = Sphere(pos, size.x() / 2.f, new FlatColor(Vec3(1.f, 1.f, 1.f)));
	}

	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const {
		return mSphere.Hit(r, t_min, t_max, rec);
	}
	virtual Vec3 RandInLight() {
		return mBoundingBox.Center() + RandInSphere() * mBoundingBox.GetSize().x(); // Sphere should have all 3 directions the same size
	}

	Sphere mSphere;
};

class BoxLight : public Light {
public:
	BoxLight(Vec3 const& pos, Vec3 const& size, float const intensity) {
		mIntensity = intensity;
		mBoundingBox.Expand(pos + size / 2.f);
		mBoundingBox.Expand(pos - size / 2.f);
	}

	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const {
		return mBoundingBox.Hit(r);
	}
	virtual Vec3 RandInLight() {
		return mBoundingBox.mMax - Vec3(RandFloat(), RandFloat(), RandFloat()) * mBoundingBox.GetSize();
	}

	//Cube mCube;
};

// Box light
// Sphere light clas
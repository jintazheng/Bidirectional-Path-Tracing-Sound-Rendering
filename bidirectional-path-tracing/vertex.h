#pragma once

#include "vec3.h"

class Vertex {

public:
	Vertex() {}
	Vertex(Vec3 const& p, Vec3 const& t, Vec3 const& n) : mPos(p), mTexCoords(t), mNorm(n) {}

	Vec3 mPos;
	Vec3 mTexCoords; // Only first 2 used
	Vec3 mNorm;
};
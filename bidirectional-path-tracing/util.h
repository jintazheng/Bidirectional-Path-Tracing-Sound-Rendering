#pragma once

#include "vec3.h"

namespace {
	std::default_random_engine generator(time(NULL));
	std::normal_distribution<double> distribution(0.f, 1.f);
}

template<typename T> 
bool isType(void* inp) {
	return dynamic_cast<T>(inp);
}

float RandFloat() {
	return (float)rand() / (float)(RAND_MAX - 1);
}

Vec3 RandOnSphere() {
	static Vec3 p;
	p[0] = distribution(generator);
	p[1] = distribution(generator);
	p[2] = distribution(generator);
	p.normalize();
	return p;
}

Vec3 RandOnDisk() {
	static Vec3 p;
	p[0] = distribution(generator);
	p[1] = distribution(generator);
	p[2] = 0;
	p.normalize();
	return p;
}

Vec3 RandInSphere() {
	Vec3 p;
	do {
		p = 2.0 * Vec3(RandFloat(), RandFloat(), RandFloat()) - Vec3(1, 1, 1);
	} while (dot(p, p) >= 1.0f);
	return p;
}


Vec3 RandInDisk() {
	Vec3 p;
	do {
		p = 2.0 * Vec3(RandFloat(), RandFloat(), 0) - Vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0f);
	return p;
}


Vec3 Reflect(Vec3 const& incident, Vec3 const& normal) {
	return incident - 2 * dot(incident, normal) * normal;
}

bool Refract(Vec3 const& incident, Vec3 const& normal, float ratio_of_indicies, Vec3& refracted) {
	Vec3 unitIncident = incident.unitVec();
	float dt = dot(unitIncident, normal);
	float discriminant = 1.0 - ((ratio_of_indicies * ratio_of_indicies) * (1 - (dt * dt)));
	if (discriminant > 0) {
		refracted = ratio_of_indicies * (unitIncident - (normal * dt)) - (normal * sqrt(discriminant));
		return true;
	}
	return false;
}

float Schlick(float const cosine, float const ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow(1 - cosine, 5);
}
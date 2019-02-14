#pragma once

#include "vec3.h"
#include "Util.h"

class Material {
public:
	Material(Vec3 const color) : drawColor(color) {}
	virtual bool scatter() const = 0;

	void SetDrawColor() {
		glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0, 0, 0));
		glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(drawColor.x(), drawColor.y(), drawColor.z()));
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(drawColor.x(), drawColor.y(), drawColor.z()));
		glMaterialfv(GL_FRONT, GL_SPECULAR, Array3(0.f, 0.f, 0.f));
		glMaterialf(GL_FRONT, GL_SHININESS, 10);
	}

	Vec3 drawColor;
};

class Solid : public Material {
public:
	virtual bool scatter() const {
		return false;
	}
	Solid(Vec3 const attenuation, Vec3 const color) : mAttenuation(attenuation), Material(color) {}

	Vec3 mAttenuation;
};

class Listener : public Material {
public:
	Listener(Vec3 const color) : Material(color) {}

	virtual bool scatter() const {
		return false;
	}
	// Indicates the listener has been hit
};

/*class FlatColor : public Material {
public:
		
	FlatColor(Vec3 const& col) : mColor(col) {}
	virtual bool scatter(Ray const& r_in, HitRecord const& rec, float& scatterAmount, Ray& scattered) const {
		return false; // Single colors do not scatter
	}

	Vec3 mColor;
};*/

/*class Lambertian : public Material {
public:
	Lambertian(Vec3 const& a) : albedo(a) {}

	virtual bool scatter(Ray const& r_in, HitRecord const& rec, Vec3& attenuation, Ray& scattered) const {
		Vec3 target = rec.p + rec.normal + RandInSphere();
		scattered = Ray(rec.p, target - rec.p);
		attenuation = albedo;
		return true;
	}

	Vec3 albedo;
};

class Specular : public Material {
public:
	Specular(Vec3 const& a, float const s, float const i, float(f)) : albedo(a), shinyness(s), intensity(i), fuzz(f) {}

	virtual bool scatter(Ray const& r_in, HitRecord const& rec, Vec3& attenuation, Ray& scattered) const {
		Vec3 reflected = Reflect(r_in.direction().unitVec(), rec.normal);
		reflected += fuzz * RandInSphere();

		// Calculate how much of the ray should come from reflection, how much should come from diffuse
		float const cosPhi = dot(rec.normal, reflected) / rec.normal.length() / reflected.length();
		float const spec = pow(cosPhi, shinyness) * intensity;

		if (RandFloat() < spec) {
			// Reflect
			scattered = Ray(rec.p, reflected);
			attenuation = Vec3(spec, spec, spec);
		}
		else {
			// Base color
			Vec3 target = rec.p + rec.normal + RandInSphere();
			scattered = Ray(rec.p, target - rec.p);
			attenuation = albedo;
		}

		return true;
	}

	Vec3 albedo;
	float shinyness;
	float intensity;
	float fuzz;
};*/

/*class Metal : public Material {
public:
	Metal(Vec3 const& a, float const f) : albedo(a), fuzz(f) {}

	virtual bool scatter(Ray const& r_in, HitRecord const& rec, Vec3& attenuation, Ray& scattered) const {
		Vec3 reflected = Reflect(r_in.direction().unitVec(), rec.normal);
		scattered = Ray(rec.p, reflected + fuzz * RandInSphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

	Vec3 albedo;
	float fuzz;
};


class Dielectric : public Material {
public:
	Dielectric(float const ri) : ref_idx(ri) {}

	virtual bool scatter(Ray const& r_in, HitRecord const& rec, Vec3& attenuation, Ray& scattered) const {
		Vec3 outward_normal;
		Vec3 reflected = Reflect(r_in.direction(), rec.normal);
		Vec3 refracted;
		float ratio_of_indicies;
		attenuation = Vec3(1.0, 1.0, 1.0);
		float reflect_prob;
		float cosine;

		// Calculate the normal based on if the ray is inside or outside the sphere
		if (dot(r_in.direction(), rec.normal) > 0) {
			outward_normal = rec.normal * -1;
			ratio_of_indicies = ref_idx;
			cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		else {
			outward_normal = rec.normal;
			ratio_of_indicies = 1.0f / ref_idx;
			cosine = -1 * dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}

		// Refract the ray
		if (Refract(r_in.direction(), outward_normal, ratio_of_indicies, refracted)) {
			reflect_prob = Schlick(cosine, ref_idx);
		}
		else {
			scattered = Ray(rec.p, reflected);
			reflect_prob = 1.0f;
		}

		// Decide to reflect or refract randomly
		if (RandFloat() < reflect_prob) {
			scattered = Ray(rec.p, reflected);
		}
		else {
			scattered = Ray(rec.p, refracted);
		}
		return true;

	}

	float ref_idx;
};*/
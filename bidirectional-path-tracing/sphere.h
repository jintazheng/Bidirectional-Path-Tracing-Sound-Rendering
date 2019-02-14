#pragma once

#include "object.h"
#include "SphereDraw.h"
#include "material.h"

class Sphere : public Object {
public:
	Sphere() {}
	Sphere(Vec3 const cen, float const r, Material* m) : center(cen), radius(r), material(m) {
		mBoundingBox.Expand(center + Vec3(radius, radius, radius));
		mBoundingBox.Expand(center - Vec3(radius, radius, radius));
	};

	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const
	{
		Vec3 const oc = r.origin() - center;
		float const a = dot(r.direction(), r.direction());
		float const b = dot(oc, r.direction());
		float const c = dot(oc, oc) - radius * radius;
		float const discriminant = b * b - a * c;

		if (discriminant > 0) {
			float soln = (-b - sqrt(discriminant)) / a;
			if (soln < t_max && soln > t_min) {
				rec.t = soln;
				rec.p = r.point_at_parameter(rec.t);
				rec.normal = (rec.p - center) / radius;
				rec.material = material;
				return true;
			}
			soln = (-b + sqrt(discriminant)) / a;
			if (soln < t_max && soln > t_min) {
				rec.t = soln;
				rec.p = r.point_at_parameter(rec.t);
				rec.normal = (rec.p - center) / radius;
				rec.material = material;
				return true;
			}
		}
		return false;
	}

	virtual void Draw() {
		material->SetDrawColor();
		//glColor3f(material->drawColor.x(), material->drawColor.y(), material->drawColor.z());
		MjbSphere(radius, center.x(), center.y(), center.z(), 20, 20);
	}

	Vec3 center;
	float radius;
	Material* material;
};


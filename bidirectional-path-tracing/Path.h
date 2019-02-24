#pragma once
#include "object.h"
#include <vector>
#include "vec3.h"

class Path : public Object {
public:
	Path(std::vector<Vec3>& path, Material* m) : material(m), mPath(path) { }

	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const {
		return false;
	}

	virtual void Draw() {
		glDisable(GL_LIGHTING);
		material->SetDrawColor();
		glLineWidth(material->drawColor.x() * 3.f);
		glBegin(GL_LINE_STRIP);
		for (auto it = mPath.begin(); it != mPath.end(); ++it) {
			glVertex3f(it->x(), it->y(), it->z());
		}
		glEnd();
		glEnable(GL_LIGHTING);
	}

	std::vector<Vec3> mPath;
	Material* material;
};
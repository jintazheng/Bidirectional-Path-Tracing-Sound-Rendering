#pragma once

#include "object.h"
#include <vector>

class AccelerationStructure : public Object {
public:
	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const = 0;
};

class HitableList : public AccelerationStructure {
public:
	HitableList() {}
	HitableList(Object ** l, int n) { list = l; list_size = n; }
	HitableList(std::vector<Object*> const& l) {
		list_size = l.size();
		list = new Object*[list_size];
		for (int i = 0; i < list_size; ++i) {
			list[i] = l[i];
		}
	}

	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const
	{
		HitRecord temp_record;
		bool hit_anything = false;
		double closest = t_max;
		for (int ii = 0; ii < list_size; ++ii) {
			if (list[ii]->Hit(r, t_min, closest, temp_record)) {
				hit_anything = true;
				closest = temp_record.t;
				rec = temp_record;
			}
		}

		return hit_anything;
	}

	virtual void Draw() {
		for (int ii = 0; ii < list_size; ++ii) {
			list[ii]->Draw();
		}
	}

	Object **list;
	int list_size;
};
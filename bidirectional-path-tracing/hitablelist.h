#pragma once

#include "hitable.h"
#include <vector>

class HitableList : public Hitable {
public:
	HitableList() {}
	HitableList(Hitable ** l, int n) { list = l; list_size = n; }
	HitableList(std::vector<Hitable*> const& l) {
		list_size = l.size();
		list = new Hitable*[list_size];
		for (int i = 0; i < list_size; ++i) {
			list[i] = l[i];
		}
	}
	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const;

	Hitable **list;
	int list_size;
};

bool HitableList::Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const
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
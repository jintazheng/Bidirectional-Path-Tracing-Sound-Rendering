#pragma once
#include "triangle.h"
#include "object.h"
#include "Box.h"
#include "BVH.h"

int const maxElementsPerLeaf = 8;
int const maxTreeDepth = 100;

class Octree : public AccelerationStructure {
public:
	int GetOctant(Vec3 const& octCenter, Vec3 const& triCenter) {
		if (triCenter.x() >= octCenter.x() && triCenter.y() >= octCenter.y() && triCenter.z() >= octCenter.z()) //+++ (0)
			return 0;
		if (triCenter.x() < octCenter.x() && triCenter.y() >= octCenter.y() && triCenter.z() >= octCenter.z()) //-++ (1)
			return 1;
		if (triCenter.x() < octCenter.x() && triCenter.y() < octCenter.y() && triCenter.z() >= octCenter.z()) //--+ (2)
			return 2;
		if (triCenter.x() >= octCenter.x() && triCenter.y() < octCenter.y() && triCenter.z() >= octCenter.z()) //+-+ (3)
			return 3;
		if (triCenter.x() >= octCenter.x() && triCenter.y() >= octCenter.y() && triCenter.z() < octCenter.z()) //++- (4)
			return 4;
		if (triCenter.x() < octCenter.x() && triCenter.y() >= octCenter.y() && triCenter.z() < octCenter.z()) //-+- (5)
			return 5;
		if (triCenter.x() >= octCenter.x() && triCenter.y() < octCenter.y() && triCenter.z() < octCenter.z()) //+-- (6)
			return 6;
		else  //--- (7)
			return 7;
	}

	Octree(std::vector<Object*> const& objIn, int d) : depth(d) {

		// Find the size of this node and allocate the array
		mElementsCount = objIn.size();
		mElements = new Object*[mElementsCount];
		for (int ii = 0; ii < mElementsCount; ++ii) {
			mElements[ii] = objIn.at(ii);
			mBoundingBox.Expand(mElements[ii]->mBoundingBox);
		}

		isLeaf = true;

		// Determine if it is worth splitting this tree
		if (mElementsCount < maxElementsPerLeaf || depth > maxTreeDepth) {
			return;
		}


		// 8 octants for splitting
		std::vector<Object*> temp[8];

		// Prepare to create child nodes
		Vec3 const& octCenter = mBoundingBox.Center();
		for (int jj = 0; jj < mElementsCount; ++jj) {
			int const octant = GetOctant(octCenter, mElements[jj]->mBoundingBox.Center());
			temp[octant].push_back(mElements[jj]);
		}

		// Create the child nodes
		for (int ii = 0; ii < 8; ++ii) {
			if (temp[ii].size()) {
				child[ii] = new Octree(temp[ii], depth + 1);
				isLeaf = false;
			} else {
				child[ii] = nullptr;
			}
		}
	}

	bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const {
		// Check if volume is hit at all
		if (!HitBB(r)) {
			return false;
		}

		HitRecord temp_record;
		bool hit_anything = false;
		double closest = t_max;

		// Loop through all elements if this is a leaf
		if (isLeaf) {
			for (int ii = 0; ii < mElementsCount; ++ii) {
				if (mElements[ii]->Hit(r, t_min, closest, temp_record)) {

					hit_anything = true;
					closest = temp_record.t;
					rec = temp_record;
				}
			}
		} else {
			// Loop through all children otherwise
			for (int ii = 0; ii < 8; ++ii) {
				if (!child[ii]) {
					continue;
				}
				if (child[ii]->Hit(r, t_min, closest, temp_record)) { // Need to get the t_value to produce a priority queue

					hit_anything = true;
					closest = temp_record.t;
					rec = temp_record;
				}
			}
		}

		return hit_anything;
	}

	int depth;
	bool isLeaf;
	Octree* child[8];

	uint32_t mElementsCount;
	Object** mElements;
};
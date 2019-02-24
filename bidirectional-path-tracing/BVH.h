#pragma once

#include "AccelerationStructure.h"
#include "triangle.h"
#include <algorithm>
#include <vector>
#include "Box.h"

int const minElementsPerLeaf = 3;
int const maxBVHDepth = 100;

bool compareTrianglesX(Triangle* first, Triangle* second) {
	return first->mBoundingBox.mMin.x() < second->mBoundingBox.mMin.x();
}
bool compareTrianglesY(Triangle* first, Triangle* second) {
	return first->mBoundingBox.mMin.y() < second->mBoundingBox.mMin.y();
}
bool compareTrianglesZ(Triangle* first, Triangle* second) {
	return first->mBoundingBox.mMin.z() < second->mBoundingBox.mMin.z();
}

class BVH : public AccelerationStructure {
public:
	BVH() {
		mDepth = 0;
		mRight = nullptr;
		mLeft = nullptr;
		mElementsCount = 0;
		mElements = nullptr;
	}

	BVH(std::vector<Triangle*> const& inElements, int const d) : mDepth(d) {
		mElementsCount = inElements.size();
		mElements = new Triangle*[mElementsCount];
		for (int ii = 0; ii < inElements.size(); ++ii) {
			mBoundingBox.Expand(inElements[ii]->mBoundingBox);
			mElements[ii] = inElements[ii];
		}

		// Determine this is a leaf node
		if (mElementsCount <= minElementsPerLeaf || mDepth > maxBVHDepth) {
			mIsLeaf = true;
			return;
		}

		// Split the tree into 2 children accross the major axis of the bounding box
		// Need to find a better method to split evenly
		mIsLeaf = false;
		Vec3 const& center = mBoundingBox.Center();
		int const majorAxis = mBoundingBox.GetMajorAxis();

		// Sort smallest to largest
		switch (majorAxis) {
		case X_AXIS:
			std::sort(mElements, mElements + mElementsCount, compareTrianglesX);
			break;
		case Y_AXIS:
			std::sort(mElements, mElements + mElementsCount, compareTrianglesY);
			break;
		case Z_AXIS:
			std::sort(mElements, mElements + mElementsCount, compareTrianglesZ);
			break;
		}

		std::vector<Triangle*> temp[2];
		for (int ii = 0; ii < mElementsCount; ++ii) {
			if (ii < mElementsCount / 2) { // Split in half
				temp[0].push_back(inElements[ii]);
			} else {
				temp[1].push_back(inElements[ii]);
			}
		}

		if (temp[0].size() > 0) {
			mLeft = new BVH(temp[0], mDepth + 1);
		} else {
			mLeft = nullptr;
		}
		if (temp[1].size() > 0) {
			mRight = new BVH(temp[1], mDepth + 1);
		} else {
			mRight = nullptr;
		}
	}

	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const
	{
		// Check if volume is hit at all
		if (!HitBB(r)) {
			return false;
		}

		HitRecord temp_record;
		bool hit_anything = false;
		double closest = t_max;

		// Loop through all elements if this is a leaf
		if (mIsLeaf) {
			for (int ii = 0; ii < mElementsCount; ++ii) {
				if (mElements[ii]->Hit(r, t_min, closest, temp_record)) {

					hit_anything = true;
					closest = temp_record.t;
					rec = temp_record;
				}
			}
		} else {
			// Call children.hit otherwise
			if (mLeft) {
				if (mLeft->Hit(r, t_min, closest, temp_record)) {
					hit_anything = true;
					closest = temp_record.t;
					rec = temp_record;
				}
			}
			if (mRight) {
				if (mRight->Hit(r, t_min, closest, temp_record)) {
					hit_anything = true;
					closest = temp_record.t;
					rec = temp_record;
				}
			}
		}

		return hit_anything;
	}

	virtual void Translate(Vec3 const& trans) {
		Object::Translate(trans);
		if (mLeft) {
			mLeft->Translate(trans);
		}
		if (mRight) {
			mRight->Translate(trans);
		}
	}

	virtual void Draw() {
		for (int ii = 0; ii < mElementsCount; ++ii) {
			mElements[ii]->Draw();
		}
	}

	bool mIsLeaf;
	Triangle** mElements;
	int mElementsCount;

	int mDepth;
	BVH* mLeft;
	BVH* mRight;
};
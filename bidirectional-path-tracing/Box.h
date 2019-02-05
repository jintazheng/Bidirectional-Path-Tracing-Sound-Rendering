#pragma once

enum {
	X_AXIS = 0,
	Y_AXIS = 1,
	Z_AXIS = 2,
};

class Box {
public:
	Box() {
		mMin = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		mMax = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	}
	Box(Vec3 const& min, Vec3 const& max) : mMin(min), mMax(max) {}

	Vec3 Center() {
		return (mMax + mMin) / 2.f;
	}

	void Expand(Vec3 const& pos) {
		mMax = Vec3(fmax(pos.x(), mMax.x()),
			fmax(pos.y(), mMax.y()),
			fmax(pos.z(), mMax.z()));
		mMin = Vec3(fmin(pos.x(), mMin.x()),
			fmin(pos.y(), mMin.y()),
			fmin(pos.z(), mMin.z()));
	}

	void Expand(Box const& other) {
		mMax = Vec3(fmaxf(other.mMax.x(), mMax.x()),
			fmaxf(other.mMax.y(), mMax.y()),
			fmaxf(other.mMax.z(), mMax.z()));
		mMin = Vec3(fminf(other.mMin.x(), mMin.x()),
			fminf(other.mMin.y(), mMin.y()),
			fminf(other.mMin.z(), mMin.z()));
	}

	Vec3 GetSize() {
		return mMax - mMin;
	}

	int GetMajorAxis() {
		float widthx = mMax.x() - mMin.x();
		float widthy = mMax.y() - mMin.y();
		float widthz = mMax.z() - mMin.z();

		if (widthx > widthy && widthx > widthz) {
			return X_AXIS;
		}
		if (widthy > widthx && widthy > widthz) {
			return Y_AXIS;
		}
		return Z_AXIS;
	}

	bool Hit(Ray const& r) const {
		float tmin, tmax, tymin, tymax;

		if (r.sign[0]) {
			tmin = (mMin.x() - r.origin().x()) * r.invDir.x();
			tmax = (mMax.x() - r.origin().x()) * r.invDir.x();
		} else { // Swapped order
			tmax = (mMin.x() - r.origin().x()) * r.invDir.x();
			tmin = (mMax.x() - r.origin().x()) * r.invDir.x();
		}

		if (r.sign[1]) {
			tymin = (mMin.y() - r.origin().y()) * r.invDir.y();
			tymax = (mMax.y() - r.origin().y()) * r.invDir.y();
		} else { // Swapped order
			tymax = (mMin.y() - r.origin().y()) * r.invDir.y();
			tymin = (mMax.y() - r.origin().y()) * r.invDir.y();
		}
		
		if (tmin > tymax || tymin > tmax) {
			return false;
		}

		tmin = fmax(tymin, tmin);
		tmax = fmin(tmax, tymax);

		float tzmin, tzmax;

		if (r.sign[2]) {
			tzmin = (mMin.z() - r.origin().z()) * r.invDir.z();
			tzmax = (mMax.z() - r.origin().z()) * r.invDir.z();
		} else { // Swapped order
			tzmax = (mMin.z() - r.origin().z()) * r.invDir.z();
			tzmin = (mMax.z() - r.origin().z()) * r.invDir.z();
		}

		if (tmin > tzmax || tzmin > tmax) {
			return false;
		}

		tmin = fmax(tmin, tzmin);
		tmax = fmin(tmax, tzmax); // Need to return these values

		return true;
	}

	void Translate(Vec3 const& trans) {
		mMin += trans;
		mMax += trans;
	}


	Vec3 mMin;
	Vec3 mMax;
};
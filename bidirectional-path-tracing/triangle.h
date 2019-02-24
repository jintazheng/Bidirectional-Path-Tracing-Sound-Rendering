#pragma once

#include "object.h"

class Vertex {

public:
	Vertex() {}
	Vertex(Vec3 const& p, Vec3 const& t, Vec3 const& n) : mPos(p), mTexCoords(t), mNorm(n) {}

	Vec3 mPos;
	Vec3 mTexCoords; // Only first 2 used
	Vec3 mNorm;
};

class Triangle : public Object {
public:
	Triangle() {}
	Triangle(Vec3 const& a, Vec3 const& b, Vec3 const& c, Material* m) : material(m) {
		Vec3 zero(0, 0, 0);
		Vec3 normal = cross(b - a, c - a);
		A = Vertex(a, zero, normal);
		B = Vertex(b, zero, normal);
		C = Vertex(c, zero, normal);
		Init();
	};
	Triangle(Vertex const& a, Vertex const& b, Vertex const& c, Material* m) : A(a), B(b), C(c), material(m) {
		Init();
	};

	void Init() {
		// Precompute common values
		mBoundingBox = Box();
		mBoundingBox.Expand(A.mPos);
		mBoundingBox.Expand(B.mPos);
		mBoundingBox.Expand(C.mPos);

		N = cross(B.mPos - A.mPos, C.mPos - A.mPos);
		Area = N.length() / 2.0f;
		NHat = normalize(N);
		AB = B.mPos - A.mPos;
		AC = C.mPos - A.mPos;
		dABAB = dot(AB, AB);
		dACAC = dot(AC, AC);
		dABAC = dot(AB, AC);
		denominator = 1.f / (dABAB * dACAC - dABAC * dABAC);
	}

	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const
	{
		// Project r onto the plane of the Polygon
		float const t = dot(A.mPos - r.origin(), NHat) / dot(r.direction(), NHat);

		// Check the t bounds
		if (t > t_max || t < t_min) {
			return false;
		}

		// Check if the hit location is within the triangle
		// Convert to barycentric coordinates
		Vec3 const& I = r.point_at_parameter(t);

		Vec3 toCenter = I - A.mPos;
		float distCent = dot(toCenter, AB);
		float distCent2 = dot(toCenter, AC);
		float a = (dACAC * distCent - dABAC * distCent2) * denominator;
		float b = (dABAB * distCent2 - dABAC * distCent) * denominator;
		float c = 1.f - a - b;

		if (a < 0.f || b < 0.f || c < 0.f || a > 1.f || b > 1.f || c > 1.f) {
			return false;
		}

		// TODO: Store barycentric coordinates in hit
		rec.t = t;
		rec.p = I;
		rec.normal = NHat;
		rec.material = material;
		return true;
	}

	void Translate(Vec3 const& trans) {
		A.mPos += trans;
		B.mPos += trans;
		C.mPos += trans;
		Init();
	}

	virtual void Draw(sf::Shader* shader) {
		//glColor3f(material->drawColor.x(), material->drawColor.y(), material->drawColor.z());
		sf::Shader::bind(shader);
		material->SetDrawColor(shader);
		glBegin(GL_TRIANGLES);
		glNormal3f(A.mNorm.x(), A.mNorm.y(), A.mNorm.z());
		glVertex3f(A.mPos.x(), A.mPos.y(), A.mPos.z());
		glNormal3f(B.mNorm.x(), B.mNorm.y(), B.mNorm.z());
		glVertex3f(B.mPos.x(), B.mPos.y(), B.mPos.z());
		glNormal3f(C.mNorm.x(), C.mNorm.y(), C.mNorm.z());
		glVertex3f(C.mPos.x(), C.mPos.y(), C.mPos.z());
		glEnd();
	}

	Vertex A;
	Vertex B;
	Vertex C;
	Material* material;

	Vec3 N;
	float Area;
	Vec3 NHat;
	Vec3 AB;
	float dABAB;
	Vec3 AC;
	float dACAC;
	float dABAC;
	float denominator;
};
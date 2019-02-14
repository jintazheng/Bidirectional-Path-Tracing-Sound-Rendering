#pragma once
#include "assimp/scene.h"
#include "triangle.h"
#include "BVH.h"
#include <vector>

class Mesh : public Object
{
public:
	Mesh(aiMesh const* mesh, aiMaterial const* material) {
		aiVector3D const Zero3D(0.0f, 0.0f, 0.0f);

		// Store the verticies
		for (uint32_t ii = 0; ii < mesh->mNumVertices; ++ii) {
			aiVector3D const* pPos = &(mesh->mVertices[ii]);
			aiVector3D const* pNormal = &(mesh->mNormals[ii]) == NULL ? &Zero3D : &(mesh->mNormals[ii]);
			aiVector3D const* pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][ii]) : &Zero3D;

			mVerticies.push_back(Vertex(Vec3(pPos->x, pPos->y, pPos->z),
				Vec3(pTexCoord->x, pTexCoord->y, 0),
				Vec3(pNormal->x, pNormal->y, pNormal->z)));

			// Update the bounding box to include this vertex
			mBoundingBox.Expand(Vec3(pPos->x, pPos->y, pPos->z));
		}

		// Store the indicies
		for (uint32_t ii = 0; ii < mesh->mNumFaces; ++ii) {
			aiFace const& face = mesh->mFaces[ii];
			if (face.mNumIndices != 3) {
				continue;
			}
			mIndicies.push_back(face.mIndices[0]);
			mIndicies.push_back(face.mIndices[1]);
			mIndicies.push_back(face.mIndices[2]);
		}

		// Create the triangles
		uint32_t a, b, c;
		for (uint32_t ii = 0; ii < mIndicies.size(); ii += 3) {
			a = mIndicies[ii + 0];
			b = mIndicies[ii + 1];
			c = mIndicies[ii + 2];
			Triangle* temp = new Triangle(mVerticies[a], mVerticies[b], mVerticies[c], nullptr); /* material set for entire mesh */
			mTriangles.push_back(temp);
		}

		// Create the acceleration structure
		mAccelerationStructure = new BVH(mTriangles, 0);
	}

	void Translate(Vec3 const& trans) {
		Object::Translate(trans);
		for (int jj = 0; jj < mTriangles.size(); ++jj) {
			mTriangles[jj]->Translate(trans);
		}
		mAccelerationStructure->Translate(trans);
	}

	virtual bool Hit(Ray const& r, float const t_min, float const t_max, HitRecord& rec) const {
		if (mAccelerationStructure->Hit(r, t_min, t_max, rec)) {
			rec.material = material;
			return true;
		}
		return false;
	}

	virtual void Draw() {
		glBegin(GL_TRIANGLES);
		for (auto it = mTriangles.begin(); it != mTriangles.end(); ++it) {
			(*it)->Draw();
		}
	}

	// Structures only used for creating the polygons later
	std::vector<Vertex> mVerticies;
	std::vector<uint32_t> mIndicies;
	std::vector<Triangle*> mTriangles;

	BVH* mAccelerationStructure;

	Material* material;
};
#pragma once

#include <string>
#include "mesh.h"
#include <fstream>

#include "assimp/scene.h"


class Model
{
public:
	Model(aiScene const* mObject) {
		mMeshCount = mObject->mNumMeshes;
		mMeshes = new Mesh*[mMeshCount];

		// Initialize scene meshes one by one
		for (uint32_t ii = 0; ii < mMeshCount; ++ii) {
			aiMesh const* tempMesh = mObject->mMeshes[ii];
			Mesh* newMesh = new Mesh(tempMesh, mObject->mMaterials[tempMesh->mMaterialIndex]);
			mMeshes[ii] = newMesh;
		}
	}

	void AddMeshes(std::vector<Object*>& objectList, Vec3 const& trans, Material* materialOverride = nullptr) {
		for (int ii = 0; ii < mMeshCount; ++ii) {
			// Translate
			// TODO: Rotate, Scale
			mMeshes[ii]->Translate(trans);

			// Replace material
			if (materialOverride) {
				mMeshes[ii]->material = materialOverride;
			}

			// Add mesh to list
			objectList.push_back(mMeshes[ii]);
		}
	}

	Mesh** mMeshes;
	uint32_t mMeshCount = 0;

	//Texture* mTextures;
	//uint32_t mTextureCount = 0;
};
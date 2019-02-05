#pragma once
#include "Model.h"
#include "object.h"
#include <string>
#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"


class ModelLoader {

public:
	Model* LoadModel(std::string const& filename) {

		Assimp::Importer importer;

		// Check if the file exists
		std::ifstream infile(filename);
		if (!infile.fail()) {
			infile.close();
		}
		else {
			printf("Couldn't open file: %s\n", filename.c_str());
			printf("%s\n", importer.GetErrorString());
			return nullptr;
		}

		aiScene const* mScene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);
		// TODO: Use ASSIMP to translate/scale models

		// Check if import failed
		if (!mScene)
		{
			printf("%s\n", importer.GetErrorString());
			return nullptr;
		}
			
		return new Model(mScene);
	}

};
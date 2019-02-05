#pragma once

#include "Light.h"
#include "Octree.h"

class World {
public:
	World() {
		mOctree = nullptr;
		mLights = nullptr;
		mLightCount = 0;
	}

	World(std::vector<Object*> objects, std::vector<Light*> lights) {
		// Add all lights to the object list
		for (Light* l : lights) {
			objects.push_back((Object*)l);
		}

		mOctree = new Octree(objects, 0);
		mLightCount = lights.size();
		mLights = new Light*[mLightCount];
		for (int ii = 0; ii < mLightCount; ++ii) {
			mLights[ii] = lights[ii];
		}
	}

	Octree* mOctree;
	Light** mLights;
	int mLightCount;
};
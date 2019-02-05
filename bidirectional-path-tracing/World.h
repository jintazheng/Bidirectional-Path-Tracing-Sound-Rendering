#pragma once

#include "Octree.h"

class World {
public:
	World() {
		mOctree = nullptr;
	}

	World(std::vector<Object*> objects) {
		mOctree = new Octree(objects, 0);
	}

	Octree* mOctree;
};
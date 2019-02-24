#pragma once

#include "SoundNode.h"
#include "Octree.h"

class World {
public:
	World() {
		mOctree = nullptr;
	}

	World(std::vector<Object*> collidingObjects, std::vector<Object*> nonCollidingObjects) {
		mOctree = new Octree(collidingObjects, 0);

		// Store all objects that don't collide in a vector
		mNonCollidingObjects = nonCollidingObjects;
	}

	void AddNonCollidingObject(Object* object) {
		mNonCollidingObjects.push_back(object);
	}

	Octree* mOctree;
	std::vector<Object*> mNonCollidingObjects;
};
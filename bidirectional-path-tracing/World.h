#pragma once

#include "SoundNode.h"
#include "Octree.h"

class World {
public:
	World() {
		mOctree = nullptr;
		mSounds = nullptr;
		mSoundCount = 0;
	}

	World(std::vector<Object*> objects, std::vector<SoundNode*> sounds) {
		mOctree = new Octree(objects, 0);

		// Store sound objects in an array
		mSoundCount = sounds.size();
		mSounds = new SoundNode*[mSoundCount];
		for (int ii = 0; ii < mSoundCount; ++ii) {
			mSounds[ii] = sounds[ii];
		}
	}

	Octree* mOctree;
	SoundNode** mSounds;
	int mSoundCount;
};
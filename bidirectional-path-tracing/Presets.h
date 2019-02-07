#pragma once

enum Preset {
	kBox,
};

void LoadPreset(World** world, Preset const p) {
	switch (p) {
	case kBox: {
		std::vector<Object*> objects;

		Material* m = new Solid(Vec3(0.95f, 0.95f, 0.95f));

		int roomSize = 5; /* meters */

		// Floor
		objects.push_back(new Triangle(Vec3(-roomSize, -roomSize, roomSize), Vec3(roomSize, -roomSize, roomSize), Vec3(roomSize, -roomSize, -roomSize), m));
		objects.push_back(new Triangle(Vec3(roomSize, -roomSize, -roomSize), Vec3(-roomSize, -roomSize, -roomSize), Vec3(-roomSize, -roomSize, roomSize), m));

		// Left wall
		objects.push_back(new Triangle(Vec3(-roomSize, -roomSize, roomSize), Vec3(-roomSize, -roomSize, -roomSize), Vec3(-roomSize, roomSize, roomSize), m));
		objects.push_back(new Triangle(Vec3(-roomSize, -roomSize, -roomSize), Vec3(-roomSize, roomSize, -roomSize), Vec3(-roomSize, roomSize, roomSize), m));

		// Right wall
		objects.push_back(new Triangle(Vec3(roomSize, -roomSize, roomSize), Vec3(roomSize, roomSize, roomSize), Vec3(roomSize, -roomSize, -roomSize), m));
		objects.push_back(new Triangle(Vec3(roomSize, roomSize, roomSize), Vec3(roomSize, roomSize, -roomSize), Vec3(roomSize, -roomSize, -roomSize), m));

		// back wall
		objects.push_back(new Triangle(Vec3(-roomSize, -roomSize, -roomSize), Vec3(roomSize, -roomSize, -roomSize), Vec3(roomSize, roomSize, -roomSize), m));
		objects.push_back(new Triangle(Vec3(roomSize, roomSize, -roomSize), Vec3(-roomSize, roomSize, -roomSize), Vec3(-roomSize, -roomSize, -roomSize), m));

		// front wall
		objects.push_back(new Triangle(Vec3(roomSize, -roomSize, roomSize), Vec3(-roomSize, -roomSize, roomSize), Vec3(roomSize, roomSize, roomSize), m));
		objects.push_back(new Triangle(Vec3(-roomSize, roomSize, roomSize), Vec3(roomSize, roomSize, roomSize), Vec3(-roomSize, -roomSize, roomSize), m));

		// Ceiling
		objects.push_back(new Triangle(Vec3(-roomSize, roomSize, roomSize), Vec3(roomSize, roomSize, -roomSize), Vec3(roomSize, roomSize, roomSize), m));
		objects.push_back(new Triangle(Vec3(roomSize, roomSize, -roomSize), Vec3(-roomSize, roomSize, roomSize), Vec3(-roomSize, roomSize, -roomSize), m));


		std::vector<SoundNode*> sounds;
		sounds.push_back(new SoundNode(Vec3(0, 0, -4)));

		//listener
		objects.push_back(new Sphere(Vec3(0, 0, 4), 0.25f, new Listener()));

		*world = new World(objects, sounds);

		return;
	}
	default:
		return;
	}
}
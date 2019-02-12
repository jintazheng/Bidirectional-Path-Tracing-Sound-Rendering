#pragma once

enum Preset {
	kBox,
};

void LoadPreset(World** world, Preset const p) {
	switch (p) {
	case kBox: {
		std::vector<Object*> objects;

		Material* m = new Solid(Vec3(0.99f, 0.99f, 0.99f));
		Material* floor = new Solid(Vec3(0.8f, 0.8f, 0.8f));

		// Size in each direction
		float x = 10.f; // meters
		float y = 5.f;
		float z = 10.f;

		// Front vertices
		Vec3 const fTopLeft  = Vec3(-x, y, z);
		Vec3 const fTopRight = Vec3(x, y, z);
		Vec3 const fBottomLeft = Vec3(-x, -y, z);
		Vec3 const fBottomRight = Vec3(x, -y, z);

		// Back vertices
		Vec3 const bTopLeft = Vec3(-x, y, -z);
		Vec3 const bTopRight = Vec3(x, y, -z);
		Vec3 const bBottomLeft = Vec3(-x, -y, -z);
		Vec3 const bBottomRight = Vec3(x, -y, -z);

		// Floor
		objects.push_back(new Triangle(fBottomLeft, fBottomRight, bBottomLeft, floor));
		objects.push_back(new Triangle(bBottomRight, bBottomLeft, fBottomRight, floor));

		// Left wall
		objects.push_back(new Triangle(fTopLeft, fBottomLeft, bTopLeft, m));
		objects.push_back(new Triangle(bBottomLeft, bTopLeft, fBottomLeft, m));

		// Right wall
		objects.push_back(new Triangle(fTopRight, bTopRight, fBottomRight, m));
		objects.push_back(new Triangle(bBottomRight, fBottomRight, bTopRight, m));

		// back wall
		objects.push_back(new Triangle(bTopLeft, bBottomLeft, bTopRight, m));
		objects.push_back(new Triangle(bBottomRight, bTopRight, bBottomLeft, m));

		// front wall
		objects.push_back(new Triangle(fTopLeft, fTopRight, fBottomLeft, m));
		objects.push_back(new Triangle(fBottomRight, fBottomLeft, fTopRight, m));

		// Ceiling
		objects.push_back(new Triangle(fTopLeft, bTopLeft, fTopRight, m));
		objects.push_back(new Triangle(bTopRight, fTopRight, bTopLeft, m));


		std::vector<SoundNode*> sounds;
		sounds.push_back(new SoundNode(Vec3(-8, 0, -8)));

		//listener
		objects.push_back(new Sphere(Vec3(8, 0, 8), 0.4f, new Listener()));

		*world = new World(objects, sounds);

		return;
	}
	default:
		return;
	}
}
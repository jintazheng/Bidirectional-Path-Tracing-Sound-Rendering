#pragma once

enum Preset {
	kRandomScene,
	kChapter10,
	kLighting,
	kShapes,
	kModel,
	kMirror,
	kShadow,
};

void LoadPreset(World** world, Camera** camera, int const width, int const height, Preset const p) {
	switch (p) {
	/*case kRandomScene: {
		std::vector<Object*> objects;

		// Floor
		objects.push_back(new Sphere(Vec3(0, -1000, 0), 1000, new Lambertian(Vec3(0.5, 0.5, 0.5))));

		for (int a = -11; a < 11; ++a) {
			for (int b = -11; b < 11; ++b) {
				float const material = RandFloat();

				// Center in a random 11 / 11 grid
				Vec3 center(a + 0.9 * RandFloat(), 0.2, b + 0.9 * RandFloat());

				if ((center - Vec3(4, 0, 2.0)).length() > 0.9f) {
					if (material < 0.8) { // Diffuse
						objects.push_back(new Sphere(center, 0.2, new Lambertian(Vec3(RandFloat() * RandFloat(), RandFloat() * RandFloat(), RandFloat() * RandFloat()))));
						//list[current++] = new Sphere(center, 0.2, new Specular(Vec3(RandFloat() * RandFloat(), RandFloat() * RandFloat(), RandFloat() * RandFloat()), RandFloat() * 100.f, RandFloat(), RandFloat() * RandFloat() * RandFloat() * RandFloat()));
					}
					else if (material < 0.95) { // Metal
						objects.push_back(new Sphere(center, 0.2, new Metal(Vec3(0.5*(1 + RandFloat()), 0.5*(1 + RandFloat()), 0.5*(1 + RandFloat())), 0.0f)));
					}
					else { // Glass
						objects.push_back(new Sphere(center, 0.2, new Dielectric(1.5f)));
					}

				}
			}
		}

		// Main shapes
		objects.push_back(new Sphere(Vec3(0, 1, 0), 1.0f, new Dielectric(1.5f)));
		objects.push_back(new Sphere(Vec3(-4, 1, 0), 1.0f, new Lambertian(Vec3(0.4, 0.2, 0.1))));
		objects.push_back(new Sphere(Vec3(4, 1, 0), 1.0f, new Metal(Vec3(0.7, 0.6, 0.5), 0.0f)));

		std::vector<Light*> lights;

		*world = new World(objects, lights);

		// Set camera
		Vec3 cameraLocation(10, 1.5, 3);
		Vec3 lookAt(0, 0, 0);
		float focal_distance = (cameraLocation - Vec3(4, 1, 0)).length();
		float aperture = 0.1f;
		float fov = 30.f;

		*camera = new Camera(cameraLocation, lookAt, Vec3(0, 1, 0), fov, (float)width / (float)height, aperture, focal_distance);
		return;
	}
	case kChapter10: {
		std::vector<Object*> objects;

		objects.push_back(new Sphere(Vec3(0, 0, -1), 0.5, new Lambertian(Vec3(0.1, 0.2, 0.5))));
		objects.push_back(new Sphere(Vec3(0, -100.5, -1), 100, new Lambertian(Vec3(0.8, 0.8, 0.0))));
		objects.push_back(new Sphere(Vec3(1, 0, -1), 0.5, new Metal(Vec3(0.8, 0.6, 0.2), 0.0f)));
		objects.push_back(new Sphere(Vec3(-1, 0, -1), 0.5, new Dielectric(1.5f)));
		objects.push_back(new Sphere(Vec3(-1, 0, -1), -.45, new Dielectric(1.5f)));

		std::vector<Light*> lights;

		*world = new World(objects, lights);

		Vec3 cameraLocation(-2, 2, 1);
		Vec3 lookAt(0, 0, -1);
		float focal_distance = (cameraLocation - lookAt).length();
		float aperture = 0.f;
		float fov = 25.f;

		*camera = new Camera(cameraLocation, lookAt, Vec3(0, 1, 0), fov, (float)width / (float)height, aperture, focal_distance);
		return;
	}
	case kLighting: {
		std::vector<Object*> objects;

		// Floor
		objects.push_back(new Sphere(Vec3(0, -1000, 0), 1000, new Lambertian(Vec3(0.5, 0.5, 0.5))));

		objects.push_back(new Sphere(Vec3(-1, 0.5, 0), 0.5f, new Specular(Vec3(0.8, 0.5, 0.2), 5, 0.5f, 0.01f)));
		objects.push_back(new Sphere(Vec3(0, 0.5, 0), 0.5f, new Specular(Vec3(0., 0.8, 0.), 30, 1.0f, 0.05f)));
		objects.push_back(new Sphere(Vec3(1, 0.5, 0), 0.5f, new Specular(Vec3(0.3, 1., 0.4), 50, 1.0f, 0.1f)));
		objects.push_back(new Sphere(Vec3(-1, 0.5, 1), 0.3f, new Specular(Vec3(1.0, 0., 0.), 70, 1.0f, 0.05f)));
		objects.push_back(new Sphere(Vec3(1, 0.5, 1), 0.3f, new Specular(Vec3(0., 1., 1.), 90, 0.5f, 0.00f)));
		objects.push_back(new Sphere(Vec3(0, 0.5, 1), 0.3f, new Dielectric(1.5f)));

		// Something behind camera
		//objects.push_back(new Sphere(Vec3(1, 4, 4), 1.5f, new Dielectric(1.5f)));
		objects.push_back(new Sphere(Vec3(1, 4, 4), 1.5f, new Lambertian(Vec3(0.7, 0.7, 0.7))));

		std::vector<Light*> lights;

		*world = new World(objects, lights);

		// Set camera location
		Vec3 cameraLocation(0, 2, 2);
		Vec3 lookAt(0, 0, 0);
		float focal_distance = (cameraLocation - lookAt).length();
		float aperture = 0.01f;
		float fov = 50.f;

		*camera = new Camera(cameraLocation, lookAt, Vec3(0, 1, 0), fov, (float)width / (float)height, aperture, focal_distance);
		return;
	}
	case kShapes: {
		std::vector<Object*> objects;

		ModelLoader loader;
		Model* box1 = loader.LoadModel("Models/cube.obj");
		box1->AddMeshes(objects, Vec3(0, 1, 0), new Lambertian(Vec3(0.9f, 0.3f, 0.5f)));
		Model* box2 = loader.LoadModel("Models/cube.obj");
		box2->AddMeshes(objects, Vec3(-3, 1, 0), new Lambertian(Vec3(0.3f, 0.9f, 0.5f)));
		Model* box3 = loader.LoadModel("Models/cube.obj");
		box3->AddMeshes(objects, Vec3(1, 1, -3), new Metal(Vec3(0.8, 0.8, 0.8), 0.0f));

		// Floor
		objects.push_back(new Triangle(Vec3(50, 0, 50), Vec3(0, 0, -50), Vec3(-50, 0, 50), new Lambertian(Vec3(0.5, 0.5, 0.5))));
		objects.push_back(new Sphere(Vec3(-2, 1, -3), 1.f, new Metal(Vec3(0.8, 0.8, 0.6), 0.0f)));

		std::vector<Light*> lights;

		*world = new World(objects, lights);

		// Set camera location
		Vec3 cameraLocation(2.4f, 6, 6);
		Vec3 lookAt(0, 1.2f, 0);
		float focal_distance = (cameraLocation - lookAt).length();
		float aperture = 0.f;
		float fov = 30.f;

		*camera = new Camera(cameraLocation, lookAt, Vec3(0, 1, 0), fov, (float)width / (float)height, aperture, focal_distance);
		return;
	}
	case kModel: {
		std::vector<Object*> objects;

		ModelLoader loader;
		Model* monkey = loader.LoadModel("Models/Monkey.obj");
		monkey->AddMeshes(objects, Vec3(0, 3, 0), new Lambertian(Vec3(94 / 255.f, 84 / 255.f, 43 / 255.f)));

		// Floor
		objects.push_back(new Triangle(Vec3(50, 0, 50), Vec3(0, 0, -50), Vec3(-50, 0, 50), new Lambertian(Vec3(0.5, 0.5, 0.5))));

		std::vector<Light*> lights;

		*world = new World(objects, lights);

		// Set camera location
		Vec3 cameraLocation(0, 3, 5);
		Vec3 lookAt(0, 3, 0);
		float focal_distance = (cameraLocation - lookAt).length();
		float aperture = 0.f;
		float fov = 30.f;

		*camera = new Camera(cameraLocation, lookAt, Vec3(0, 1, 0), fov, (float)width / (float)height, aperture, focal_distance);
		return;
	}
	case kMirror: {
		std::vector<Object*> objects;

		// Floor
		objects.push_back(new Sphere(Vec3(0, -1000, 0), 1000, new Lambertian(Vec3(0.5, 0.5, 0.5))));
		objects.push_back(new Triangle(Vec3(2, 0, -30), Vec3(2, 0, 30), Vec3(2, 30, 0), new Metal(Vec3(0.9, 0.9, 0.9), 0.0)));
		objects.push_back(new Triangle(Vec3(-2, 0, 30), Vec3(-2, 0, -30), Vec3(-2, 30, 0), new Metal(Vec3(0.9, 0.9, 0.9), 0.0)));
		objects.push_back(new Sphere(Vec3(0, 1, 0), 1, new Lambertian(Vec3(0.9, 0.5, 0.5))));

		std::vector<Light*> lights;

		*world = new World(objects, lights);

		// Set camera location
		Vec3 cameraLocation(0, 1, 3.f);
		Vec3 lookAt(5, 1, 0);
		float focal_distance = (cameraLocation - lookAt).length();
		float aperture = 0.f;
		float fov = 50.f;

		*camera = new Camera(cameraLocation, lookAt, Vec3(0, 1, 0), fov, (float)width / (float)height, aperture, focal_distance);
		return;
	}*/
	case kShadow: {
		std::vector<Object*> objects;

		float const wallShiny = 15.f;
		Vec3 const wallSpec = Vec3(0.1, 0.1, 0.1);

		// Floor
		objects.push_back(new Triangle(Vec3(-1, 0, 5), Vec3(1, 0, 5), Vec3(1, 0, -1), new Solid(Vec3(0.6f, 0.6f, 0.6f), wallSpec, wallShiny)));
		objects.push_back(new Triangle(Vec3(1, 0, -1), Vec3(-1, 0, -1), Vec3(-1, 0, 5), new Solid(Vec3(0.6f, 0.6f, 0.6f), wallSpec, wallShiny)));

		// Left wall
		objects.push_back(new Triangle(Vec3(-1, 0, 5), Vec3(-1, 0, -1), Vec3(-1, 2, 5), new Solid(Vec3(183.f / 255.f, 33.f / 255.f, 33.f / 255.f), wallSpec, wallShiny)));
		objects.push_back(new Triangle(Vec3(-1, 0, -1), Vec3(-1, 2, -1), Vec3(-1, 2, 5), new Solid(Vec3(183.f / 255.f, 33.f / 255.f, 33.f / 255.f), wallSpec, wallShiny)));

		// Right wall
		objects.push_back(new Triangle(Vec3(1, 0, 5), Vec3(1, 2, 5), Vec3(1, 0, -1), new Solid(Vec3(40.f / 255.f, 145.f / 255.f, 24.f / 255.f), wallSpec, wallShiny)));
		objects.push_back(new Triangle(Vec3(1, 2, 5), Vec3(1, 2, -1), Vec3(1, 0, -1), new Solid(Vec3(40.f / 255.f, 145.f / 255.f, 24.f / 255.f), wallSpec, wallShiny)));

		// back wall
		objects.push_back(new Triangle(Vec3(-1, 0, -1), Vec3(1, 0, -1), Vec3(1, 2, -1), new Solid(Vec3(0.6f, 0.6f, 0.6f), wallSpec, wallShiny)));
		objects.push_back(new Triangle(Vec3(1, 2, -1), Vec3(-1, 2, -1), Vec3(-1, 0, -1), new Solid(Vec3(0.6f, 0.6f, 0.6f), wallSpec, wallShiny)));

		// front wall
		objects.push_back(new Triangle(Vec3(1, 0, 5), Vec3(-1, 0, 5), Vec3(1, 2, 5), new Solid(Vec3(0.6f, 0.6f, 0.6f), wallSpec, wallShiny)));
		objects.push_back(new Triangle(Vec3(-1, 2, 5), Vec3(1, 2, 5), Vec3(-1, 0, 5), new Solid(Vec3(0.6f, 0.6f, 0.6f), wallSpec, wallShiny)));

		// Ceiling
		objects.push_back(new Triangle(Vec3(-1, 2, 5), Vec3(1, 2, -1), Vec3(1, 2, 5), new Solid(Vec3(0.6f, 0.6f, 0.6f), wallSpec, wallShiny))); // TODO: Why are these black?
		objects.push_back(new Triangle(Vec3(1, 2, -1), Vec3(-1, 2, 5), Vec3(-1, 2, -1), new Solid(Vec3(0.6f, 0.6f, 0.6f), wallSpec, wallShiny)));

		ModelLoader l;
		Model* cube1 = l.LoadModel("Models/Cube45.obj");
		cube1->AddMeshes(objects, Vec3(0.4, -0.2f, 0), new Solid(Vec3(0.6f, 0.6f, 0.6f), Vec3(0.6f, 0.6f, 0.6f), 5.f));

		std::vector<Light*> lights;
		lights.push_back(new SphereLight(Vec3(0, 1.8f, 0), Vec3(0.2, 0.2f, 0.2), 1.f));

		*world = new World(objects, lights);

		// Set camera location
		Vec3 cameraLocation(0, 1, 4);
		Vec3 lookAt(0, 1, 0);
		float focal_distance = (cameraLocation - lookAt).length();
		float aperture = 0.f;
		float fov = 40.f;

		*camera = new Camera(cameraLocation, lookAt, Vec3(0, 1, 0), fov, (float)width / (float)height, aperture, focal_distance);
		return;
	}
	default:
		return;
	}
}
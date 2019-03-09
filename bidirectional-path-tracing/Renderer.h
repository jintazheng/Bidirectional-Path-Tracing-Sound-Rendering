#pragma once
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <glm.hpp> // vec2, vec3, mat4, radians
#include <ext.hpp> // perspective, translate, rotate

namespace
{
	// Window variables
	int windowWidth = 800;
	int windowHeight = 600;
	char const * const windowTitle = "Ray tracing";
	bool mouseTrap = false;
	unsigned int MSAALevel = 0;
	bool postAAEnabled = false;

	// Movement variables
	float yaw = 0.;
	float pitch = 0.;
	float const sens = 0.01;
	float const moveSense = 0.1f;
	glm::mat4 translation = glm::mat4(1.0f);

	// Timing variables
	float lastFrametime = 0.f;
	std::chrono::high_resolution_clock::time_point lastClock;
	bool benchmarkMode = false;
	std::vector<float> benchmarkFrametimes;
}


void RenderThread(World* world) {
	sf::Shader adsLighting;
	sf::Shader postAA;

	// load both shaders
	if (!adsLighting.loadFromFile("Shaders/ads.vert", "Shaders/ads.frag")){
		printf("Failed to load shader 'adsLighting'");
		return;
	}
	if (!postAA.loadFromFile("Shaders/KAA.vert", "Shaders/KAA.frag")) {
		printf("Failed to load shader 'postAA'");
		return;
	}

	// load a font
	sf::Font font;
	if (!font.loadFromFile("Fonts/arial.ttf")){
		printf("Failed to load font 'arial.ttf'");
		return;
	}


	// create the window
	sf::ContextSettings settings = sf::ContextSettings(24, 0, MSAALevel, 1, 1, 0, false);
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), windowTitle, sf::Style::Default, settings);
	//window.setVerticalSyncEnabled(true);

	// activate the window
	window.setActive(true);

	// Move the mouse to the center of the screen and hide it
	if (mouseTrap) {
		glm::vec2 centerScreen = glm::vec2(windowWidth / 2.f + window.getPosition().x, windowHeight / 2.f + window.getPosition().y);
		sf::Mouse::setPosition(sf::Vector2i(centerScreen.x, centerScreen.y));
		window.setMouseCursorVisible(false);
	}

	bool running = true;
	while (running)
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				// end the program
				running = false;
				break;
			case sf::Event::Resized:
				// adjust the viewport when the window is resized
				windowWidth = event.size.width;
				windowHeight = event.size.height;
				glViewport(0, 0, windowWidth, windowHeight);
				window.setView(sf::View(sf::FloatRect(0, 0, windowWidth, windowHeight)));
				break;
			case sf::Event::MouseMoved:
				if (!mouseTrap)
					break;
				glm::vec2 lastMousePos = glm::vec2(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
				glm::vec2 centerScreen = glm::vec2(windowWidth / 2 + window.getPosition().x, windowHeight / 2 + window.getPosition().y);
				sf::Mouse::setPosition(sf::Vector2i(centerScreen.x, centerScreen.y));
				glm::vec2 mouseDelta = lastMousePos - centerScreen;
				yaw += mouseDelta.x * sens;
				pitch += mouseDelta.y * sens;
				pitch = glm::clamp(pitch, -(float)M_PI_2, (float)M_PI_2);
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == 1)
				{
					// Right click to enable mouse
					mouseTrap = false;
					window.setMouseCursorVisible(true);
				}
				if (event.mouseButton.button == 0) {
					// Left click to trap mouse
					mouseTrap = true;
					window.setMouseCursorVisible(false);
					glm::vec2 centerScreen = glm::vec2(windowWidth / 2 + window.getPosition().x, windowHeight / 2 + window.getPosition().y);
					sf::Mouse::setPosition(sf::Vector2i(centerScreen.x, centerScreen.y));
				}
				break;
			case sf::Event::KeyPressed:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
					running = false;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
					postAAEnabled = !postAAEnabled;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2)) {
					if (MSAALevel == 0) {
						MSAALevel = 2;
					}
					else if (MSAALevel < 16) {
						MSAALevel *= 2;
					}
					else {
						MSAALevel = 0;
					}
					settings = sf::ContextSettings(24, 0, MSAALevel, 1, 1, 0, false);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3)) {
					if (benchmarkMode) {
						// Print the results of the benchmark
						float totalTime = 0.f;
						for (auto it = benchmarkFrametimes.begin(); it != benchmarkFrametimes.end(); ++it) {
							totalTime += *it;
						}
						float samples = (float)benchmarkFrametimes.size();
						printf("Benchmark results: %f frames, %fms avg, %ffps avg\n", samples, totalTime / samples, samples / totalTime * 1000.f);
						benchmarkFrametimes.clear();
					}
					benchmarkMode = !benchmarkMode;
				}
				break;
			default:
				break;
			}
		}

		sf::RenderTexture pass1;
		pass1.create(windowWidth, windowHeight, settings);
		pass1.setActive();


		// clear the buffers
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// setup projection
		glMatrixMode(GL_PROJECTION);
		glm::mat4 projection = glm::perspective(glm::radians(90.f), (float)windowWidth / (float)windowHeight, 0.001f, 1000.f);
		glLoadMatrixf(glm::value_ptr(projection));

		// setup camera
		glMatrixMode(GL_MODELVIEW);
		glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -2), glm::vec3(0, 1, 0));
		view = glm::rotate(view, pitch, glm::vec3(1, 0, 0));
		view = glm::rotate(view, yaw, glm::vec3(0, 1, 0));
		glLoadMatrixf(glm::value_ptr(view));


		// Handle movement
		glm::vec3 forward(view[0][2], view[1][2], view[2][2]);
		glm::vec3 strafe(view[0][0], view[1][0], view[2][0]);
		float dx = 0;
		float dz = 0;
		if (mouseTrap) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				dz = moveSense;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				dx = moveSense;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				dz = -moveSense;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				dx = -moveSense;
			}
		}

		translation = glm::translate(translation, dz * forward);
		translation = glm::translate(translation, dx * strafe);


		glLoadMatrixf(glm::value_ptr(view * translation));

		// Draw the scene
		sf::Shader::bind(&adsLighting);
		adsLighting.setUniform("uLightPosition", sf::Glsl::Vec4(0, 0, 0, 1));
		//adsLighting.setUniform("uEyePosition", sf::Glsl::Vec4(translation[3][0], translation[3][1], translation[3][2], 1.f));   // Passing eye position to counteract???
		adsLighting.setUniform("uKa", 0.2f);
		adsLighting.setUniform("uKd", 1.f);
		adsLighting.setUniform("uKs", 0.f);  // TODO: fix specular lighting
		adsLighting.setUniform("uShininess", 50.f);
		world->mOctree->Draw(&adsLighting);
		for (int ii = 0; ii < world->mNonCollidingObjects.size(); ++ii) {
			LockWorld();
			world->mNonCollidingObjects[ii]->Draw(&adsLighting);
			UnlockWorld();
		}
		sf::Shader::bind(NULL);

		// display the texture
		pass1.display();

		// activate the window
		window.setActive(true);

		// clear the buffers
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// setup orthographic projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0., windowWidth, windowHeight, 0, 0.1, 1000.);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Load the first pass into a texture
		sf::Texture initialPass = pass1.getTexture();
		sf::Shader::bind(&postAA);
		postAA.setUniform("uImageUnit", initialPass);      // * indicates defualt values
		postAA.setUniform("uContrastThreshold", 0.0625f);  // 0.0833 *0.0625 0.0312
		postAA.setUniform("uRelativeThreshold", 0.166f);   // 0.333 0.250 *0.166 0.125 0.063
		postAA.setUniform("uBlurAmount", 0.75f);           // 1.00 *0.75 0.50 0.25 0.00
		postAA.setUniform("uEnabled", postAAEnabled);	   // enable / disable

		// Draw a fullscreen quad using the first pass
		glBegin(GL_QUADS);
			glNormal3f(0, 0, 1);

			glTexCoord2f(0, 0); glVertex3f(0, 0, -1.f);
			glTexCoord2f(1, 0); glVertex3f(windowWidth, 0, -1.f);
			glTexCoord2f(1, 1); glVertex3f(windowWidth, windowHeight, -1.f);
			glTexCoord2f(0, 1); glVertex3f(0, windowHeight, -1.f);
		glEnd();

		char buffer[64] = {};
		snprintf(buffer, 64, "%.1f ms\n%.1f fps\nMSAA %ux", lastFrametime, (1.f/lastFrametime) * 1000.f, MSAALevel);
		if (postAAEnabled) {
			strncat_s(buffer, "\nPost AA", 9);
		}
		if (benchmarkMode) {
			strncat_s(buffer, "\nBenchmarking...", 17);
		}
		sf::Text text(buffer, font, 32);
		text.setFillColor(sf::Color(255, 0, 0, 255));
		window.pushGLStates();
		window.draw(text);
		window.popGLStates();

        // end the current frame (internally swaps the front and back buffers)
        window.display();

		// Calculate the frametime
		auto endFrame = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> fp_ms = endFrame - lastClock;
		lastFrametime = fp_ms.count();
		lastClock = endFrame;
		if (benchmarkMode) {
			benchmarkFrametimes.push_back(lastFrametime);
		}
    }
}
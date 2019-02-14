#pragma once
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <glm.hpp> // vec2, vec3, mat4, radians
#include <ext.hpp> // perspective, translate, rotate

namespace
{
	int screenWidth = 800;
	int screenHeight = 600;

	float yaw = 0.;
	float pitch = 0.;

	bool mouseDown = false;
	float const sens = 0.01;
	float const moveSense = 0.1f;
	glm::mat4 translation = glm::mat4(1.0f);
}


void RenderThread(World* world) {
	// create the window
    sf::Window window(sf::VideoMode(screenWidth, screenHeight), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);

    // activate the window
    window.setActive(true);

	// Move the mouse to the center of the screen and hide it
	glm::vec2 centerScreen = glm::vec2(screenWidth / 2.f + window.getPosition().x, screenHeight / 2.f + window.getPosition().y);
	sf::Mouse::setPosition(sf::Vector2i(centerScreen.x, centerScreen.y));
	window.setMouseCursorVisible(false);

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
					glViewport(0, 0, event.size.width, event.size.height);
					screenWidth = event.size.width;
					screenHeight = event.size.height;
					break;
				case sf::Event::MouseMoved:
					glm::vec2 lastMousePos = glm::vec2(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
					glm::vec2 centerScreen = glm::vec2(screenWidth / 2.f + window.getPosition().x, screenHeight / 2.f + window.getPosition().y);
					sf::Mouse::setPosition(sf::Vector2i(centerScreen.x, centerScreen.y));
					glm::vec2 mouseDelta = lastMousePos - centerScreen;
					yaw += mouseDelta.x * sens;
					pitch += mouseDelta.y * sens;
					pitch = glm::clamp(pitch, -(float)M_PI_2, (float)M_PI_2);
					break;
				default:
					break;
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			running = false;
		}

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// setup projection
       	glMatrixMode(GL_PROJECTION);
		glm::mat4 projection = glm::perspective(glm::radians(90.f), (float)screenWidth / (float)screenHeight, 0.001f, 1000.f);
		glLoadMatrixf(glm::value_ptr(projection));

		// setup camera
		glMatrixMode(GL_MODELVIEW);
		glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -2), glm::vec3(0, 1, 0));
		view = glm::rotate(view, pitch, glm::vec3(1, 0, 0));
		view = glm::rotate(view, yaw, glm::vec3(0, 1, 0));
		glLoadMatrixf(glm::value_ptr(view));

		glm::vec3 forward(view[0][2], view[1][2], view[2][2]);
		glm::vec3 strafe(view[0][0], view[1][0], view[2][0]);
		float dx = 0;
		float dz = 0;
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

		translation = glm::translate(translation, dz * forward);
		translation = glm::translate(translation, dx * strafe);

		// Handle movement
		/*float sinX = sin(yaw) * moveSense;
		float cosX = cos(yaw) * moveSense;
		float sinXStrafe = sin(yaw + M_PI_2) * moveSense;
		float cosXStrafe = cos(yaw + M_PI_2)  * moveSense; // Cleaner way to do this using rotation matrix
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			translation = glm::translate(translation, glm::vec3(-sinX , 0, cosX));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			translation = glm::translate(translation, glm::vec3(sinXStrafe, 0, -cosXStrafe));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			translation = glm::translate(translation, glm::vec3(sinX, 0, -cosX));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			translation = glm::translate(translation, glm::vec3(-sinXStrafe, 0, cosXStrafe));
		}*/

		glLoadMatrixf(glm::value_ptr(view * translation));

		// setup scene
		glEnable(GL_NORMALIZE);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHTING);

		// set up light
		glLightfv(GL_LIGHT0, GL_POSITION, Array3(0, 0, 0));
		glLightfv(GL_LIGHT0, GL_AMBIENT, Array3(0.2, 0.2, 0.2));
		glLightfv(GL_LIGHT0, GL_DIFFUSE, Array3(1, 1, 1));
		glLightfv(GL_LIGHT0, GL_SPECULAR, Array3(1, 1, 1));
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.);
		glEnable(GL_LIGHT0);

		// Draw the scene
		world->mOctree->Draw();	
		for (int ii = 0; ii < world->mSoundCount; ++ii) {
			world->mSounds[ii]->Draw();
		}

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);

        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }
}
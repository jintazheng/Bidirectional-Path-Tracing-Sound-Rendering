// bidirectional-path-tracing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "vec3.h"
#include "ray.h"
#include "Object.h"
#include "sphere.h"
#include "accelerationStructure.h"
#include "camera.h"
#include "util.h"
#include "material.h"
#include "triangle.h"
#include "mesh.h"
#include "model.h"
#include "SoundNode.h"
#include "ModelLoader.h"
#include "Octree.h"
#include "Light.h"
#include "World.h"


#include "Presets.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "3rd_party\stb\stb_image_write.h"

#include <SFML/Audio.hpp>
// TODO Add ClangFormat


int samplesPerSecond = 0;  // Global that needs to be set from file

float const reverbLength = 2.0f;
int const rays = 10000;

float const airDampeningCoeff = 0.01;


float* Convolution(float *arr1, float *arr2, int len1, int len2, int *lenResult)
{
	int ii, jj, i1;
	float temp;
	float *result;
 
	// Allocate the result array	
	*lenResult = len1 + len2 - 1;
	result = (float*) calloc(*lenResult, sizeof(float));
 
	// Convolution process
	for (ii = 0; ii < *lenResult; ii++)
	{
		i1 = ii;
		temp = 0.f;
		for (jj = 0; jj < len2; jj++)
		{
			if (i1 >= 0 && i1 < len1) {
				temp += (arr1[i1] * arr2[jj]);
			}
 
			i1--;
		}
		result[ii] = temp;
	}
  
	return result;
}

void ScaleBuffer(float* buffer, int const count, float const maxValue) {
	float max = 0.f;
	for (int ii = 0; ii < count; ++ii) {
		max = std::max(buffer[ii], max);
	}
	for (int ii = 0; ii < count; ++ii) {
		buffer[ii] /= (max / maxValue);
	}
}

void ProcessSound(float* impulseResponse, int const impulseResponseCount, sf::Int16 const* sourceSamples, int const sourceSampleCount) {
	// Normalize the impulse response such that the maximum is 1.0f
	ScaleBuffer(impulseResponse, impulseResponseCount, 1.f);

	// Convert the source samples to floats
	float * sourceBuffer = new float[sourceSampleCount];
	for (int ii = 0; ii < sourceSampleCount; ++ii) {
		sourceBuffer[ii] = (float)sourceSamples[ii];
	}

	// Mix the impulse response and the source samples
	int resultBufferCount;
	float * resultBuffer = Convolution(impulseResponse, sourceBuffer, impulseResponseCount, sourceSampleCount, &resultBufferCount);
	ScaleBuffer(resultBuffer, resultBufferCount, 32767.f); // This line shouldn't be necessary

	// Convert mixed buffer into 16 bit PCM
	sf::Int16* playableBuffer = new sf::Int16[resultBufferCount];
	for (int ii = 0; ii < resultBufferCount; ++ii) {
		playableBuffer[ii] = (sf::Int16)resultBuffer[ii];
	}

	// Play the sound
	sf::SoundBuffer buffer;
	buffer.loadFromSamples(playableBuffer, resultBufferCount, 1, samplesPerSecond);
	sf::Sound sound;
	sound.setBuffer(buffer);
	sound.play();

	// Allow the sound to play before the data is deleted
	std::this_thread::sleep_for(std::chrono::milliseconds((int)((reverbLength + 1.0f) * 1000.f)));

	delete [] playableBuffer;
	delete [] resultBuffer;
	delete [] sourceBuffer;
}


void TraceForward(Ray const& r, World* world, float* impulseResponse, float timePassed /* seconds */, Vec3 color)
{
	HitRecord rec;
	if (world->mOctree->Hit(r, 0.00001f, FLT_MAX, rec)) {
		// Calculate the time it took for the ray to reach here
		float distanceTravelled = rec.t / r.B.length();
		timePassed += GetTimeFromDistance(distanceTravelled);

		// Calculate the dampening for this distance
		float k = exp(airDampeningCoeff * distanceTravelled); // [1, inf] based on distance
		Vec3 dampening = Vec3(1.f / k, 1.f / k, 1.f / k);

		// Test if we have reached the maximum time to recieve a ray
		if (timePassed > reverbLength) {
			return;
		}

		if (isType<Listener*>(rec.material)) {
			// record the collision and return
			int sampleIndex = (int)(samplesPerSecond * timePassed);
			impulseResponse[sampleIndex] += color.x() * dampening.x(); // Only one component for now

			printf("Collision with listener at t=%f with energy=%f\n", timePassed, color.x());
			return;
		}

		if (isType<Solid*>(rec.material)) {
			Solid* mat = dynamic_cast<Solid*>(rec.material);
			Vec3 scactteredDir = rec.normal + RandInSphere();
			scactteredDir.normalize();
			Ray scattered(rec.p, scactteredDir);
			Vec3 scatteredColor = color * mat->mAttenuation * dampening;

			TraceForward(scattered, world, impulseResponse, timePassed, scatteredColor);
			return;
		}
	} else { // Ray hit nothing, sound is lost
		printf("lost ray\n");
		return;
	}
}


int main()
{
	srand(time(NULL));

	// Load the sound and gather it's raw data
	sf::SoundBuffer buffer;
	buffer.loadFromFile("Sounds/dog44k.wav");
	samplesPerSecond                   = buffer.getSampleRate();
	int              sourceSampleCount = buffer.getSampleCount();
	sf::Int16 const* sourceSamples     = buffer.getSamples();

	sf::Sound test;
	test.setBuffer(buffer);
	test.play();

	// Set up the array for impulse response
	int const impulseSampleCount = (int)(samplesPerSecond * reverbLength);
	float*    impulseResponse    = (float*)calloc(impulseSampleCount, sizeof(float));

	World* world;
	// Load a preset
	LoadPreset(&world, kBox);

	for (int sound = 0; sound < world->mSoundCount; ++sound) {
		SoundNode* origin = world->mSounds[sound];
		for (int sample = 0; sample < rays; ++sample) {
			// Choose a random direction
			Ray r(origin->mLocation, RandOnSphere());
			Vec3 origColor(1.f, 1.f, 1.f);
			TraceForward(r, world, impulseResponse, 0.f, origColor);
		}
	}


	ProcessSound(impulseResponse, impulseSampleCount, sourceSamples, sourceSampleCount);

	delete[] impulseResponse;

	return 0;
}
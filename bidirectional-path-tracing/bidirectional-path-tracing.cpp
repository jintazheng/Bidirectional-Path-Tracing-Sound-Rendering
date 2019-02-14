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
#include "AudioStream.h"
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

typedef std::chrono::high_resolution_clock Clock;

namespace {
	int samplesPerSecond = 0;  // Global that needs to be set from file
	int channels = 0;
	int const samplesPerChunk = 512;  // roughly 86.13 updates per second

	float const reverbLength = 2.0f;
	int const rays = 100;

	float const airDampingCoeff = 0.01f;
	float const minEnergy = 0.00001f;
}


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

	// Only scale if the largest value is larger than the indicated max
	if (max > maxValue) {
		for (int ii = 0; ii < count; ++ii) {
			buffer[ii] /= (max / maxValue);
		}
	}
}

void ProcessSound(float* impulseResponse, int const impulseBufferSize, float* sourceBuffer, int const sourceBufferSize, float* outputBuffer, int* outputBufferSize) {
	// Normalize the impulse response such that the maximum is 1.0f
	ScaleBuffer(impulseResponse, impulseBufferSize, 1.f);

	// Mix the impulse response and the source samples
	outputBuffer = Convolution(impulseResponse, sourceBuffer, impulseBufferSize, sourceBufferSize, outputBufferSize);
	ScaleBuffer(outputBuffer, *outputBufferSize, 32767.f); // This line shouldn't be necessary
}


void TraceForward(Ray const& r, World* world, float* impulseResponse, float timePassed /* seconds */, Vec3 color)
{
	HitRecord rec;
	if (world->mOctree->Hit(r, 0.00001f, FLT_MAX, rec)) {
		// Calculate the time it took for the ray to reach here
		float distanceTravelled = rec.t / r.B.length();
		timePassed += GetTimeFromDistance(distanceTravelled);

		// Calculate the dampening for this distance
		float k = exp(airDampingCoeff * distanceTravelled); // [1, inf] based on distance
		Vec3 damping = Vec3(1.f / k, 1.f / k, 1.f / k);

		// Test if we have reached the maximum time to recieve a ray
		if (timePassed > reverbLength) {
			return;
		}

		if (isType<Listener*>(rec.material)) {
			// record the collision and return
			int sampleIndex = (int)(samplesPerSecond * timePassed);
			impulseResponse[sampleIndex] += color.x() * damping.x(); // Only one component for now

			printf("Collision with listener at t=%f with energy=%f\n", timePassed, color.x());
			return;
		}

		if (isType<Solid*>(rec.material)) {
			Solid* mat = dynamic_cast<Solid*>(rec.material);
			Vec3 scactteredDir = rec.normal + RandInSphere();
			scactteredDir.normalize();
			Ray scattered(rec.p, scactteredDir);
			Vec3 scatteredColor = color * mat->mAttenuation * damping;

			// If this ray will have no energy, stop tracing
			if (scatteredColor.x() < minEnergy) {
				return;
			}

			TraceForward(scattered, world, impulseResponse, timePassed, scatteredColor);
			return;
		}
	} else { // Ray hit nothing, sound is lost
		printf("lost ray\n");
		return;
	}
}

void TracingThread() {

}


void SoundThread(World* world) {
	// Load the sound and gather it's raw data
	sf::SoundBuffer buffer;
	buffer.loadFromFile("Sounds/hellokirk.wav");
	samplesPerSecond = buffer.getSampleRate();
	channels = buffer.getChannelCount();
	int sourceSampleSize = buffer.getSampleCount();
	sf::Int16 const* sourceSamples     = buffer.getSamples();

	// Convert the source samples to floats
	float * sourceBuffer = new float[sourceSampleSize];
	for (int ii = 0; ii < sourceSampleSize; ++ii) {
		sourceBuffer[ii] = (float)sourceSamples[ii];
	}

	// Play the sample
	AudioStream sampleStream;
	sampleStream.init(buffer.getChannelCount(), samplesPerSecond, samplesPerChunk);
	sampleStream.addSamples(sourceSamples, sourceSampleSize);
	sampleStream.play();

	// Create the stream for playing processed sound
	AudioStream outputStream;
	outputStream.init(channels, samplesPerSecond, samplesPerChunk);

	// Set up the array for impulse response
	int const impulseSampleCount = (int)(samplesPerSecond * reverbLength);
	float*    impulseResponseBuffer = (float*)calloc(impulseSampleCount, sizeof(float));

	// Set up the array for data output, size (impulse response + chunk size)
	int outputBufferSize;
	float outputBuffer;

	// Mark when the loop starts
	auto initClock = Clock::now();
	float const secondsPerChunk = (float)samplesPerChunk / (float)samplesPerSecond;

	while (true) {
		// Get the time this iteration starts
		auto startClock = Clock::now();
		float startTime = std::chrono::duration_cast<std::chrono::seconds>(initClock - startClock).count();

		// Ray trace until no time left
		for (int sound = 0; sound < world->mSoundCount; ++sound) {
			SoundNode* origin = world->mSounds[sound];
			for (int sample = 0; sample < rays; ++sample) {
				// Choose a random direction
				Ray r(origin->mLocation, RandOnSphere());
				Vec3 origColor(1.f, 1.f, 1.f);
				TraceForward(r, world, impulseResponseBuffer, startTime, origColor);
			}
		}

		// Calculate frequency response
		// Need to have an array for remainders
		ProcessSound(impulseResponseBuffer, impulseSampleCount, sourceBuffer /* needs to shift based on time start */, samplesPerChunk, &outputBuffer, &outputBufferSize);

		// Add samples
		outputStream.addSamples(&outputBuffer, samplesPerChunk);

		if (!outputStream.Playing) {
			outputStream.play();
		}

		auto endClock = Clock::now();
		float duration = std::chrono::duration_cast<std::chrono::seconds>(startClock - endClock).count();

		if (duration > secondsPerChunk) {
			printf("Error: Missed interval by %f seconds.\n", duration - secondsPerChunk);
			continue;
		}

		while (duration < secondsPerChunk) {
			duration = std::chrono::duration_cast<std::chrono::seconds>(startClock - Clock::now()).count();
			std::this_thread::sleep_for(std::chrono::microseconds(500));
		}
		
	}

	delete[] impulseResponseBuffer;
}


int main()
{
	srand(time(NULL));

	// Load a preset
	World* world;
	LoadPreset(&world, kBox);

	std::thread audio(SoundThread, world);

	audio.join();

	return 0;
}
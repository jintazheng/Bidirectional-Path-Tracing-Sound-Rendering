#pragma once
#include <SFML/Audio.hpp>
#include <vector>

namespace {
	int const maxSamples = 441000;  // Store 10 seconds of audio
}

class AudioStream : public sf::SoundStream
{
public:

	void init(int const numChannels, int const sampleRate, int const perChunk) {
		// Initialize storage
		samplesPerChunk = perChunk;
		mSamples = new sf::Int16[maxSamples];

		// Initialize base class
		initialize(numChannels, sampleRate);
	}

	void addSamples(float* buffer, size_t const bufferSize) {
		if (mSampleCount + bufferSize > maxSamples) {
			unloadAudio(maxSamples / 2); // Unload half the samples
		}
		for (int ii = 0; ii < bufferSize; ++ii) {
			mSamples[ii + mSampleCount] = (sf::Int16)buffer[ii];
		}
		mSampleCount += bufferSize;
	}

	void addSamples(sf::Int16* buffer, size_t const bufferSize) {
		if (mSampleCount + bufferSize > maxSamples) {
			unloadAudio(maxSamples / 2); // Unload half the samples
		}
		std::copy(buffer, buffer + bufferSize, &mSamples[mSampleCount]);
		mSampleCount += bufferSize;
	}

	void addSamples(sf::Int16 const* buffer, size_t const bufferSize) {
		if (mSampleCount + bufferSize > maxSamples) {
			unloadAudio(maxSamples / 2); // Unload half the samples
		}
		std::copy(buffer, buffer + bufferSize, &mSamples[mSampleCount]);
		mSampleCount += bufferSize;
	}
private:

	void unloadAudio(size_t const count) {
		// Delete audio to unfill buffer
		std::copy(&mSamples[mCurrentSample], &mSamples[mSampleCount], &mSamples[mSampleCount - count]);
		mSampleCount -= count;
		mCurrentSample -= count;
	}

	virtual bool onGetData(Chunk& data) {
		data.samples = &mSamples[mCurrentSample];

		if (mCurrentSample + samplesPerChunk <= mSampleCount) {
			data.sampleCount = samplesPerChunk;
			mCurrentSample += samplesPerChunk;

			// There are enough samples
			return true;
		}
		else if (mCurrentSample == mSampleCount) {
			// No Samples
			printf("Error: No samples available\n");
			return false;
		} else {
			// Not enough samples, use all remaining
			printf("Error: Not enough samples\n");
			data.sampleCount = mSampleCount - mCurrentSample;
			mCurrentSample = mSampleCount;
			return true;
		}
	}

	virtual void onSeek(sf::Time timeOffset)
    {
        // Not Implemented
    }


	sf::Int16* mSamples;
	size_t mSampleCount = 0;  // The number of samples in mSamples
	size_t mCurrentSample = 0; // The next sample to be queued up in mSamples

	int samplesPerChunk = 0;
};
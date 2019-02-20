#pragma once
#include <fftw3.h>
#include <SFML/Audio.hpp>

void FillComplexArray(float* in, fftwf_complex* out, int const inCount, int const size) {
	for (int ii = 0; ii < size; ++ii) {
		if (ii < inCount) {
			out[ii][0] = in[ii];
		}
		else {
			out[ii][0] = 0;
		}
	}
}

fftwf_complex* GetComplexArray(float* in, int inCount, int size) {
	fftwf_complex* out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * size);
	for (int ii = 0; ii < size; ++ii) {
		if (ii < inCount) {
			out[ii][0] = in[ii];
		}
		else {
			out[ii][0] = 0;
		}
		out[ii][1] = 0;
	}
	return out;
}

float* GetRealArray(fftwf_complex* in, int size) {
	float* real = (float*)malloc(sizeof(float) * size);
	for (int ii = 0; ii < size; ++ii) {
		real[ii] = in[ii][0]; //+ in[ii][1];
	}
	return real;
}

void AddRealArray(fftwf_complex* in, float* out, int size) {
	for (int ii = 0; ii < size; ++ii) {
		out[ii] += in[ii][0];
	}
}

void MultArray(fftwf_complex* first, fftwf_complex* second, fftwf_complex* result, int count) {
	for (int ii = 0; ii < count; ++ii) {
		result[ii][0] = first[ii][0] * second[ii][0] - first[ii][1] * second[ii][1];
		result[ii][1] = first[ii][0] * second[ii][1] + first[ii][1] * second[ii][0];
	}
}

void FFT(fftwf_complex* in, fftwf_complex* out, int N) {
	fftwf_plan plan = fftwf_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	fftwf_execute(plan);

	fftwf_destroy_plan(plan);
	fftwf_cleanup();
}

void IFFT(fftwf_complex* in, fftwf_complex* out, int N) {
	fftwf_plan plan = fftwf_plan_dft_1d(N, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);

	fftwf_execute(plan);

	fftwf_destroy_plan(plan);
	fftwf_cleanup();

	// Scale the output for exact inverse
	for (int ii = 0; ii < N; ++ii) {
		out[ii][0] /= N;
		out[ii][1] /= N;
	}
}

float* Convolution(float *arr1, float *arr2, int len1, int len2, int *lenResult)
{
	int ii, jj, i1;
	float temp;
	float *result;

	// Allocate the result array
	*lenResult = len1 + len2 - 1;
	result = (float*)calloc(*lenResult, sizeof(float));

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

void ProcessSound(float* impulseResponse, int const impulseResponseCount, sf::Int16 const* sourceSamples, int const sourceSampleCount, int const samplesPerSecond) {
	// Normalize the impulse response such that the maximum is 1.0f
	ScaleBuffer(impulseResponse, impulseResponseCount, 1.f);

	// Convert the source samples to floats
	float * sourceBuffer = new float[sourceSampleCount];
	for (int ii = 0; ii < sourceSampleCount; ++ii) {
		sourceBuffer[ii] = (float)sourceSamples[ii];
	}

	// Mix the sounds in 3 different ways and compare their quality and times
	for (int ii = 0; ii < 4; ++ii) {
		int resultBufferCount = 0;
		float * resultBuffer = nullptr;

		auto beforeClock = std::chrono::high_resolution_clock::now();
		switch (ii) {
		case 0: // IFFT( FFT * FFT )
		{
			// Number of samples is equal to the length of the sound + length of impulse response
			int N = sourceSampleCount + impulseResponseCount - 1;

			// Allocate arrays
			fftwf_complex* sourceIn = GetComplexArray(sourceBuffer, sourceSampleCount, N);
			fftwf_complex* sourceOut = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* impulseIn = GetComplexArray(impulseResponse, impulseResponseCount, N);
			fftwf_complex* impulseOut = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* multiplied = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* result = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);

			// Compute the mixed sound
			FFT(sourceIn, sourceOut, N);
			FFT(impulseIn, impulseOut, N);
			MultArray(sourceOut, impulseOut, multiplied, N);
			IFFT(multiplied, result, N);

			// Convert the sound to float*
			resultBufferCount = N;
			resultBuffer = GetRealArray(result, N);
			ScaleBuffer(resultBuffer, resultBufferCount, 32767.f); // This line shouldn't be necessary

			// Cleanup
			fftwf_free(sourceIn);
			fftwf_free(sourceOut);
			fftwf_free(impulseIn);
			fftwf_free(impulseOut);
			fftwf_free(multiplied);
			fftwf_free(result);
		}
		break;
		case 1: // FFT segmented into source chunks
		{
			int const chunkSize = 1024;
			int const N = chunkSize + impulseResponseCount - 1;

			// Allocate arrays
			fftwf_complex* sourceIn = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* sourceOut = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* impulseIn = GetComplexArray(impulseResponse, impulseResponseCount, N);
			fftwf_complex* impulseOut = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* multiplied = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* result = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			resultBufferCount = impulseResponseCount + sourceSampleCount;
			resultBuffer = (float*)calloc(resultBufferCount, sizeof(float));

			// Impulse FFT only calculated once
			FFT(impulseIn, impulseOut, N);

			for (int jj = 0; jj * chunkSize < sourceSampleCount - chunkSize; ++jj) {
				int const offset = jj * chunkSize;

				// Fills the source buffer with chunkSize samples from the source, zero pads the end
				FillComplexArray(&sourceBuffer[offset], sourceIn, chunkSize, N);

				// Compute the mixed sound
				FFT(sourceIn, sourceOut, N);
				MultArray(sourceOut, impulseOut, multiplied, N);
				IFFT(multiplied, result, N);

				AddRealArray(result, &resultBuffer[offset], N);
			}

			ScaleBuffer(resultBuffer, resultBufferCount, 32767.f); // This line shouldn't be necessary

			// Cleanup
			fftwf_free(sourceIn);
			fftwf_free(sourceOut);
			fftwf_free(impulseIn);
			fftwf_free(impulseOut);
			fftwf_free(multiplied);
			fftwf_free(result);

		}
		break;
		case 2:
		{
			int const chunkSize = 1024;
			int const N = chunkSize + sourceSampleCount - 1;

			// Allocate arrays
			fftwf_complex* sourceIn = GetComplexArray(sourceBuffer, sourceSampleCount, N);
			fftwf_complex* sourceOut = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* impulseIn = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* impulseOut = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* multiplied = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* result = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			resultBufferCount = impulseResponseCount + sourceSampleCount;
			resultBuffer = (float*)calloc(resultBufferCount, sizeof(float));

			// Source FFT only calculated once
			FFT(sourceIn, sourceOut, N);

			for (int jj = 0; jj * chunkSize < sourceSampleCount - chunkSize; ++jj) {
				int const offset = jj * chunkSize;

				// Fills the source buffer with chunkSize samples from the source, zero pads the end
				FillComplexArray(&impulseResponse[offset], impulseIn, chunkSize, N);

				// Compute the mixed sound
				FFT(impulseIn, impulseOut, N);
				MultArray(sourceOut, impulseOut, multiplied, N);
				IFFT(multiplied, result, N);

				AddRealArray(result, &resultBuffer[offset], N);
			}

			ScaleBuffer(resultBuffer, resultBufferCount, 32767.f); // This line shouldn't be necessary

			// Cleanup
			fftwf_free(sourceIn);
			fftwf_free(sourceOut);
			fftwf_free(impulseIn);
			fftwf_free(impulseOut);
			fftwf_free(multiplied);
			fftwf_free(result);

		}
		break;
		case 3: // Convolution
			resultBuffer = Convolution(impulseResponse, sourceBuffer, impulseResponseCount, sourceSampleCount, &resultBufferCount);
			ScaleBuffer(resultBuffer, resultBufferCount, 32767.f); // This line shouldn't be necessary
			break;
		}

		// Print the time it took for this run
		auto afterClock = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> fp_ms = afterClock - beforeClock;
		printf("Calculation time: %fms\n", fp_ms.count());

		if (!resultBuffer) {
			continue;
		}

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
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(resultBufferCount / samplesPerSecond * 1000.f)));

		// Cleanup memory
		delete[] playableBuffer;
		delete[] resultBuffer;
	}

	delete[] sourceBuffer;
}

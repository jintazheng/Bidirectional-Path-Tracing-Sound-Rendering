#pragma once
#include <fftw3.h>
#include <SFML/Audio.hpp>

namespace {
	int const chunkSize = 1024;
}

void FillCompexArrayZeros(fftwf_complex* out, int const count) {
	for (int ii = 0; ii < count; ++ii) {
		out[ii][0] = 0;
		out[ii][1] = 0;
	}
}

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

void WriteBufferToCSV(fftwf_complex* buffer, int const count, std::string const& filename) {
	std::ofstream file;
	file.open(filename);
	for (int jj = 0; jj < 2; ++jj) {
		for (int ii = 0; ii < count; ++ii) {
			file << buffer[ii][jj] << ",";
		}
		file << "\n";
	}
	file.close();
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

void DivBuffer(fftwf_complex* in, int const count, float const div) {
	for (int jj = 0; jj < 2; ++jj) {
		for (int ii = 0; ii < count; ++ii) {
			in[ii][jj] /= div;
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

	// Mix the sounds in 5 different ways and compare their quality and times
	for (int ii = 0; ii < 1; ++ii) {
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

			// Write to CSV for debug
			WriteBufferToCSV(sourceIn, N, "Output/source.csv");
			WriteBufferToCSV(impulseIn, N, "Output/filter.csv");

			// Create Forward FFT plans
			fftwf_plan sourcePlan = fftwf_plan_dft_1d(N, sourceIn, sourceOut, FFTW_FORWARD, FFTW_ESTIMATE);
			fftwf_plan impulsePlan = fftwf_plan_dft_1d(N, impulseIn, impulseOut, FFTW_FORWARD, FFTW_ESTIMATE);

			// Compute the forward FFT
			fftwf_execute(sourcePlan);
			fftwf_execute(impulsePlan);

			// Multiply the 2 arrays
			MultArray(sourceOut, impulseOut, multiplied, N);

			// Compute the inverse FFT
			fftwf_plan reversePlan = fftwf_plan_dft_1d(N, multiplied, result, FFTW_BACKWARD, FFTW_ESTIMATE);
			fftwf_execute(reversePlan);

			// Write to CSV for debugs
			DivBuffer(result, N, N);
			WriteBufferToCSV(result, N, "Output/mixedSound.csv");

			// Convert the sound to float*
			resultBufferCount = N;
			resultBuffer = GetRealArray(result, N);
			//ScaleBuffer(resultBuffer, resultBufferCount, 32767.f); // This line shouldn't be necessary

			// Cleanup
			fftwf_destroy_plan(sourcePlan);
			fftwf_destroy_plan(impulsePlan);
			fftwf_destroy_plan(reversePlan);
			fftwf_free(sourceIn);
			fftwf_free(sourceOut);
			fftwf_free(impulseIn);
			fftwf_free(impulseOut);
			fftwf_free(multiplied);
			fftwf_free(result);
		}
		break;
		case 1: // FFT with source and filter segmented
		{
			int N = chunkSize;
			int totalN = ((std::max(impulseResponseCount, sourceSampleCount) + (N - 1)) / N) * N;  // Round up to nearest N

			// Allocate arrays
			fftwf_complex* sourceIn = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* sourceOut = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* impulseIn = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* impulseOut = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* multiplied = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			fftwf_complex* result = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * N);
			resultBufferCount = totalN;
			resultBuffer = (float*)calloc(resultBufferCount, sizeof(float));

			// Create plans
			fftwf_plan sourcePlan = fftwf_plan_dft_1d(N, sourceIn, sourceOut, FFTW_FORWARD, FFTW_ESTIMATE);
			fftwf_plan impulsePlan = fftwf_plan_dft_1d(N, impulseIn, impulseOut, FFTW_FORWARD, FFTW_ESTIMATE);
			fftwf_plan reversePlan = fftwf_plan_dft_1d(N, multiplied, result, FFTW_BACKWARD, FFTW_ESTIMATE);

			for (int jj = 0; jj * N < totalN; ++jj) {
				int const offset = jj * N;
				int const sourceActualSize = std::min(sourceSampleCount - offset, N);
				int const impulseActualSize = std::min(impulseResponseCount - offset, N);

				// Fills both buffers with the next N samples.  If a source has run out of samples, fill with zeros
				if (sourceActualSize > 0) {
					FillComplexArray(&sourceBuffer[offset], sourceIn, sourceActualSize, N);
				} else {
					FillCompexArrayZeros(sourceIn, N);
				}
				if (impulseActualSize > 0) {
					FillComplexArray(&impulseResponse[offset], impulseIn, impulseActualSize, N);
				} else {
					FillCompexArrayZeros(impulseIn, N);
				}

				fftwf_execute(sourcePlan);
				fftwf_execute(impulsePlan);
				MultArray(sourceOut, impulseOut, multiplied, N);
				fftwf_execute(reversePlan);

				AddRealArray(result, &resultBuffer[offset], N);
			}

			ScaleBuffer(resultBuffer, resultBufferCount, 32767.f); // This line shouldn't be necessary

			// Cleanup
			fftwf_destroy_plan(sourcePlan);
			fftwf_destroy_plan(impulsePlan);
			fftwf_destroy_plan(reversePlan);
			fftwf_free(sourceIn);
			fftwf_free(sourceOut);
			fftwf_free(impulseIn);
			fftwf_free(impulseOut);
			fftwf_free(multiplied);
			fftwf_free(result);
		}
		break;
		case 2: // FFT segmented into source chunks
		{
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

			// Create plans
			fftwf_plan sourcePlan = fftwf_plan_dft_1d(N, sourceIn, sourceOut, FFTW_FORWARD, FFTW_ESTIMATE);
			fftwf_plan impulsePlan = fftwf_plan_dft_1d(N, impulseIn, impulseOut, FFTW_FORWARD, FFTW_ESTIMATE);
			fftwf_plan reversePlan = fftwf_plan_dft_1d(N, multiplied, result, FFTW_BACKWARD, FFTW_ESTIMATE);

			// Impulse FFT only calculated once
			fftwf_execute(impulsePlan);

			for (int jj = 0; jj * chunkSize < sourceSampleCount; ++jj) {
				int const offset = jj * chunkSize;
				int const sourceChunkSize = std::min(sourceSampleCount - offset, chunkSize);

				// Fills the source buffer with chunkSize samples from the source, zero pads the end
				FillComplexArray(&sourceBuffer[offset], sourceIn, sourceChunkSize, chunkSize);

				fftwf_execute(sourcePlan);
				MultArray(sourceOut, impulseOut, multiplied, N);
				fftwf_execute(reversePlan);

				int const resultChunkSize = std::min(N, resultBufferCount - offset);
				AddRealArray(result, &resultBuffer[offset], resultChunkSize); // Possible overflow
			}

			ScaleBuffer(resultBuffer, resultBufferCount, 32767.f); // This line shouldn't be necessary

			// Cleanup
			fftwf_destroy_plan(sourcePlan);
			fftwf_destroy_plan(impulsePlan);
			fftwf_destroy_plan(reversePlan);
			fftwf_free(sourceIn);
			fftwf_free(sourceOut);
			fftwf_free(impulseIn);
			fftwf_free(impulseOut);
			fftwf_free(multiplied);
			fftwf_free(result);

		}
		break;
		case 3:
		{
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

			// Create plans
			fftwf_plan sourcePlan = fftwf_plan_dft_1d(N, sourceIn, sourceOut, FFTW_FORWARD, FFTW_ESTIMATE);
			fftwf_plan impulsePlan = fftwf_plan_dft_1d(N, impulseIn, impulseOut, FFTW_FORWARD, FFTW_ESTIMATE);
			fftwf_plan reversePlan = fftwf_plan_dft_1d(N, multiplied, result, FFTW_BACKWARD, FFTW_ESTIMATE);

			// Source FFT only calculated once
			fftwf_execute(sourcePlan);

			for (int jj = 0; jj * chunkSize < impulseResponseCount; ++jj) {
				int const offset = jj * chunkSize;
				int const impulseChunkSize = std::min(impulseResponseCount - offset, chunkSize);

				// Fills the impulse buffer with chunkSize samples from the source, zero pads the end
				FillComplexArray(&impulseResponse[offset], impulseIn, impulseChunkSize, chunkSize);

				fftwf_execute(impulsePlan);
				MultArray(sourceOut, impulseOut, multiplied, N);
				fftwf_execute(reversePlan);

				int const resultChunkSize = std::min(N, resultBufferCount - offset);
				AddRealArray(result, &resultBuffer[offset], resultChunkSize);
			}

			ScaleBuffer(resultBuffer, resultBufferCount, 32767.f); // This line shouldn't be necessary

			// Cleanup
			fftwf_destroy_plan(sourcePlan);
			fftwf_destroy_plan(impulsePlan);
			fftwf_destroy_plan(reversePlan);
			fftwf_free(sourceIn);
			fftwf_free(sourceOut);
			fftwf_free(impulseIn);
			fftwf_free(impulseOut);
			fftwf_free(multiplied);
			fftwf_free(result);

		}
		break;
		case 4: // Convolution
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

		buffer.saveToFile("Output/mixedSound.wav");

		// Allow the sound to play before the data is deleted
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(resultBufferCount / samplesPerSecond * 1000.f)));

		// Cleanup memory
		delete[] playableBuffer;
		delete[] resultBuffer;
	}

	delete[] sourceBuffer;
}

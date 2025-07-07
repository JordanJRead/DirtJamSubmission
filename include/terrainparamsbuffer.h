#ifndef TERRAIN_PARAMS_BUFFER_H
#define TERRAIN_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"

/*
int octaveCount;
float initialAmplitude;
float amplitudeDecay;
float spreadFactor;
*/

class TerrainParamsBuffer {
public:
	TerrainParamsBuffer(int octaveCount, float initialAmplitude, float amplitudeDecay, float spreadFactor);
	int* getOctaveCountPtr() { return &mOctaveCount; }
	float* getInitialAmplitudePtr() { return &mInitialAmplitude; }
	float* getAmplitudeDecayPtr() { return &mAmplitudeDecay; }
	float* getSpreadFactorPtr() { return &mSpreadFactor; }
	bool updateGPU();

private:
	BUF mBuf;

	int mOctaveCount;
	float mInitialAmplitude;
	float mAmplitudeDecay;
	float mSpreadFactor;

	int mPrevOctaveCount;
	float mPrevInitialAmplitude;
	float mPrevAmplitudeDecay;
	float mPrevSpreadFactor;

private:
	void verifyInput();
	void updateOctaveCount();
	void updateInitialAmplitude();
	void updateAmplitudeDecay();
	void updateSpreadFactor();
};

#endif
#include "terrainparamsbuffer.h"
#include "glad/glad.h"

TerrainParamsBuffer::TerrainParamsBuffer(int octaveCount, float initialAmplitude, float amplitudeDecay, float spreadFactor)
	: mOctaveCount{ octaveCount }
	, mInitialAmplitude{ initialAmplitude }
	, mAmplitudeDecay{ amplitudeDecay }
	, mSpreadFactor{ spreadFactor }

	, mPrevOctaveCount{ -1 }
	, mPrevInitialAmplitude{ -1 }
	, mPrevAmplitudeDecay{ -1 }
	, mPrevSpreadFactor{ -1 }
{
	glBindBuffer(GL_UNIFORM_BUFFER, mBuf);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 3 * sizeof(float), nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mBuf);
	updateGPU();
}

void TerrainParamsBuffer::verifyInput() {
	if (mOctaveCount < 1)
		mOctaveCount = 1;
}

bool TerrainParamsBuffer::updateGPU() {
	verifyInput();
	glBindBuffer(GL_UNIFORM_BUFFER, mBuf);
	bool hasChanged{ false };

	if (mPrevOctaveCount != mOctaveCount || true) {
		updateOctaveCount();
		mPrevOctaveCount = mOctaveCount;
		hasChanged = true;
	}

	if (mPrevInitialAmplitude != mInitialAmplitude) {
		updateInitialAmplitude();
		mPrevInitialAmplitude = mInitialAmplitude;
		hasChanged = true;
	}

	if (mPrevAmplitudeDecay != mAmplitudeDecay) {
		updateAmplitudeDecay();
		mPrevAmplitudeDecay = mAmplitudeDecay;
		hasChanged = true;
	}

	if (mPrevSpreadFactor != mSpreadFactor) {
		updateSpreadFactor();
		mPrevSpreadFactor = mSpreadFactor;
		hasChanged = true;
	}

	return hasChanged;
}

void TerrainParamsBuffer::updateOctaveCount() {
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(int), &mOctaveCount);
}

void TerrainParamsBuffer::updateInitialAmplitude() {
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(int), sizeof(float), &mInitialAmplitude);
}

void TerrainParamsBuffer::updateAmplitudeDecay() {
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(int) + sizeof(float), sizeof(float), &mAmplitudeDecay);
}

void TerrainParamsBuffer::updateSpreadFactor() {
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(int) + sizeof(float) + sizeof(float), sizeof(float), &mSpreadFactor);
}
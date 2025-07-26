#ifndef ARTISTIC_PARAMS_BUFFER_H
#define ARTISTIC_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"
#include "cpugpupair.h"

struct ArtisticParamsData {
	float terrainScale;
	float fogStrength;
	float colorDotCutoff;
	int shellCount;
	float shellMaxHeight;
	float shellDetail;
	float shellMaxCutoff;
	float shellBaseCutoff;
};

class ArtisticParamsBuffer {
public:
	ArtisticParamsBuffer(float terrainScale, float fogStrength, float colorDotCutoff, int shellCount, float shellMaxHeight, float shellDetail, float shellMaxCutoff, float shellBaseCutoff);
	ArtisticParamsBuffer(const ArtisticParamsData& data)
		: mTerrainScale{ data.terrainScale }
		, mFogStrength{ data.fogStrength }
		, mColorDotCutoff{ data.colorDotCutoff }
		, mShellCount{ data.shellCount }
		, mShellMaxHeight{ data.shellMaxHeight }
		, mShellDetail{ data.shellDetail }
		, mShellMaxCutoff{ data.shellMaxCutoff }
		, mShellBaseCutoff{ data.shellBaseCutoff }
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 7 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, mBUF);

		updateGPU(true);
	}
	void renderUI();
	void updateGPU(bool force);
	int getShellCount() const { return mShellCount.mCPU; }
	float getTerrainScale() const { return mTerrainScale.mCPU; }
	void lowerShellCount(int newShellCount) {
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		int offset{ 0 };

		int size{ sizeof(float) };
		offset += size;

		size = sizeof(float);
		offset += size;

		size = sizeof(float);
		offset += size;

		size = sizeof(int);
		mShellCount.mGPU = mShellCount.mCPU;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &newShellCount);
	}

	void fixShellCount() {
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		int offset{ 0 };

		int size{ sizeof(float) };
		offset += size;

		size = sizeof(float);
		offset += size;

		size = sizeof(float);
		offset += size;

		size = sizeof(int);
		mShellCount.mGPU = mShellCount.mCPU;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellCount.mGPU);
	}

private:
	BUF mBUF;
	CPUGPUPair<float> mTerrainScale;
	CPUGPUPair<float> mFogStrength;
	CPUGPUPair<float> mColorDotCutoff;
	CPUGPUPair<int> mShellCount;
	CPUGPUPair<float> mShellMaxHeight;
	CPUGPUPair<float> mShellDetail;
	CPUGPUPair<float> mShellMaxCutoff;
	CPUGPUPair<float> mShellBaseCutoff;
};

#endif
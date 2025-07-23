#ifndef ARTISTIC_PARAMS_BUFFER_H
#define ARTISTIC_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"
#include "shaderguipair.h"

struct ArtisticParamsData {
	float terrainScale;
	float maxFogDist;
	float colorDotCutoff;
	int shellCount;
	float shellMaxHeight;
	float shellDetail;
	float shellMaxCutoff;
	float shellBaseCutoff;
};

class ArtisticParamsBuffer {
public:
	ArtisticParamsBuffer(float terrainScale, float maxFogDist, float colorDotCutoff, int shellCount, float shellMaxHeight, float shellDetail, float shellMaxCutoff, float shellBaseCutoff);
	ArtisticParamsBuffer(const ArtisticParamsData& data)
		: mTerrainScale{ data.terrainScale }
		, mMaxFogDist{ data.maxFogDist }
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
	int getShellCount() const { return mShellCount.mGUI; }
	float getTerrainScale() const { return mTerrainScale.mGUI; }
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
		mShellCount.mShader = mShellCount.mGUI;
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
		mShellCount.mShader = mShellCount.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellCount.mShader);
	}

private:
	BUF mBUF;
	ShaderGUIPair<float> mTerrainScale;
	ShaderGUIPair<float> mMaxFogDist;
	ShaderGUIPair<float> mColorDotCutoff;
	ShaderGUIPair<int> mShellCount;
	ShaderGUIPair<float> mShellMaxHeight;
	ShaderGUIPair<float> mShellDetail;
	ShaderGUIPair<float> mShellMaxCutoff;
	ShaderGUIPair<float> mShellBaseCutoff;
};

#endif
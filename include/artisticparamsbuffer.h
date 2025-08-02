#ifndef ARTISTIC_PARAMS_BUFFER_H
#define ARTISTIC_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"
#include "shaderguipair.h"

struct ArtisticParamsData {
	float terrainScale;
	float maxViewDistance;
	float fogEncroach;
	float colorDotCutoff;
	int shellCount;
	float shellMaxHeight;
	float shellDetail;
	float shellMaxCutoff;
	float shellBaseCutoff;
	float snowHeight;
	float seafoamStrength;
};

class ArtisticParamsBuffer {
public:
	ArtisticParamsBuffer(const ArtisticParamsData& data)
		: mTerrainScale{ data.terrainScale }
		, mMaxViewDistance{ data.maxViewDistance }
		, mFogEncroach{ data.fogEncroach }
		, mColorDotCutoff{ data.colorDotCutoff }
		, mShellCount{ data.shellCount }
		, mShellMaxHeight{ data.shellMaxHeight }
		, mShellDetail{ data.shellDetail }
		, mShellMaxCutoff{ data.shellMaxCutoff }
		, mShellBaseCutoff{ data.shellBaseCutoff }
		, mSnowHeight{ data.snowHeight }
		, mSeafoamStrength{ data.seafoamStrength }
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 10 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, mBUF);

		updateGPU(true);
	}
	void renderUI();
	void updateGPU(bool force);
	int getShellCount() const { return mShellCount.mShader; }
	float getTerrainScale() const { return mTerrainScale.mGUI; }
	void forceShaderShellCount(int newShellCount) {
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		int offset{ 4 * sizeof(float) };
		int size = sizeof(int);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &newShellCount);
	}

	void fixShellCount() {
		forceShaderShellCount(mShellCount.mShader);
	}

private:
	BUF mBUF;
	ShaderGUIPair<float> mTerrainScale;
	ShaderGUIPair<float> mMaxViewDistance;
	ShaderGUIPair<float> mFogEncroach;
	ShaderGUIPair<float> mColorDotCutoff;
	ShaderGUIPair<int> mShellCount;
	ShaderGUIPair<float> mShellMaxHeight;
	ShaderGUIPair<float> mShellDetail;
	ShaderGUIPair<float> mShellMaxCutoff;
	ShaderGUIPair<float> mShellBaseCutoff;
	ShaderGUIPair<float> mSnowHeight;
	ShaderGUIPair<float> mSeafoamStrength;
};

#endif
#ifndef ARTISTIC_PARAMS_BUFFER_H
#define ARTISTIC_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"
#include "shaderguipair.h"

struct ArtisticParamsData {
	float extrudePerShell;
	float maxFogDist;
	float colordotCutoff;
	float shellTexelScale;
	float shellCutoffLossPerShell;
	float shellCutoffBase;
	int maxShellCount;
};

class ArtisticParamsBuffer {
public:
	ArtisticParamsBuffer(float extrudePerShell, float maxFogDist, float colorDotCutoff, float shellTexelScale, float shellCutoffLossPerShell, float shellCutoffBase, int maxShellCount);
	ArtisticParamsBuffer(const ArtisticParamsData& data)
		: mExtrudePerShell{ data.extrudePerShell }
		, mMaxFogDist{ data.maxFogDist }
		, mColorDotCutoff{ data.colordotCutoff }
		, mShellTexelScale{ data.shellTexelScale }
		, mShellCutoffLossPerShell{ data.shellCutoffLossPerShell }
		, mShellCutoffBase{ data.shellCutoffBase }
		, mMaxShellCount{ data.maxShellCount }
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 6 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, mBUF);

		updateGPU(true);
	}
	void renderUI();
	void updateGPU(bool force);
	int getMaxShellCount() const { return mMaxShellCount.mGUI; }

private:
	BUF mBUF;
	ShaderGUIPair<float> mExtrudePerShell;
	ShaderGUIPair<float> mMaxFogDist;
	ShaderGUIPair<float> mColorDotCutoff;
	ShaderGUIPair<float> mShellTexelScale;
	ShaderGUIPair<float> mShellCutoffLossPerShell;
	ShaderGUIPair<float> mShellCutoffBase;
	ShaderGUIPair<int> mMaxShellCount;
};

#endif
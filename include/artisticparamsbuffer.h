#ifndef ARTISTIC_PARAMS_BUFFER_H
#define ARTISTIC_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"

class ArtisticParamsBuffer {
public:
	ArtisticParamsBuffer(float extrudePerShell, float maxFogDist, float colorDotCutoff, float shellTexelScale, float shellCutoffLossPerShell, float shellCutoffBase, int maxShellCount);
	void renderUI();
	void updateGPU(bool force);
	ArtisticParamsBuffer(const ArtisticParamsBuffer& other);

private:
	template <typename T>
	struct ShaderGUIPair {
		ShaderGUIPair(T shader, T GUI) : mShader{ shader }, mGUI{ GUI } {}
		ShaderGUIPair(T value) : mShader{ value }, mGUI{ value } {}
		T mShader;
		T mGUI;

		bool hasDiff() { return shader == gui }
	};

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
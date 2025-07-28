#ifndef WATER_PARAMS_BUFFER_H
#define WATER_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"
#include "shaderguipair.h"

struct WaterParamsData {
	int waveCount;
	float initialAmplitude;
	float amplitudeMult;
	float initialFreq;
	float freqMult;
	float initialSpeed;
	float speedMult;
};

class WaterParamsBuffer {
public:
	WaterParamsBuffer(const WaterParamsData& data)
		: mInitialAmplitude{ data.initialAmplitude }
		, mWaveCount{ data.waveCount }
		, mAmplitudeMult{ data.amplitudeMult }
		, mInitialFreq{ data.initialFreq }
		, mFreqMult{ data.freqMult }
		, mInitialSpeed{ data.initialSpeed }
		, mSpeedMult{ data.speedMult }
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 6 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, mBUF);
		updateGPU(true);
	}
	void renderUI();
	bool updateGPU(bool force);

private:
	BUF mBUF;

	ShaderGUIPair<int> mWaveCount;
	ShaderGUIPair<float> mInitialAmplitude;
	ShaderGUIPair<float> mAmplitudeMult;
	ShaderGUIPair<float> mInitialFreq;
	ShaderGUIPair<float> mFreqMult;
	ShaderGUIPair<float> mInitialSpeed;
	ShaderGUIPair<float> mSpeedMult;
};

#endif
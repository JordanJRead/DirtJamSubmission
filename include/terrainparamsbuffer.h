#ifndef TERRAIN_PARAMS_BUFFER_H
#define TERRAIN_PARAMS_BUFFER_H

#include "OpenGLObjects/BUF.h"
#include "shaderguipair.h"

struct TerrainParamsData {
	int octaveCount;
	float initialAmplitude;
	float amplitudeDecay;
	float spreadFactor;
	float roughness;
};

class TerrainParamsBuffer {
public:
	TerrainParamsBuffer(const TerrainParamsData& data)
		: mOctaveCount{ data.octaveCount }
		, mInitialAmplitude{ data.initialAmplitude }
		, mAmplitudeDecay{ data.amplitudeDecay }
		, mSpreadFactor{ data.spreadFactor }
		, mRoughness{ data.roughness }
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 4 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, mBUF);
		updateGPU(true);
	}
	void renderUI();
	bool updateGPU(bool force);

private:
	BUF mBUF;

	ShaderGUIPair<int> mOctaveCount;
	ShaderGUIPair<float> mInitialAmplitude;
	ShaderGUIPair<float> mAmplitudeDecay;
	ShaderGUIPair<float> mSpreadFactor;
	ShaderGUIPair<float> mRoughness;
};

#endif
#ifndef COLOUR_BUFFER_H
#define COLOUR_BUFFER_H

#include "OpenGLObjects/BUF.h"
#include "shaderguipair.h"
#include "glm/glm.hpp"

struct ColourBufferData {
	glm::vec3 dirtColor;
	glm::vec3 mountainColor;
	glm::vec3 grassColor;
	glm::vec3 snowColor;
	glm::vec3 waterColor;
	glm::vec3 sunColor;
};

class ColourBuffer {
public:
	ColourBuffer(const ColourBufferData& data)
		: mDirtColour{ data.dirtColor }
		, mMountainColour{ data.mountainColor }
		, mGrassColour{ data.grassColor }
		, mSnowColour{ data.snowColor }
		, mWaterColour{ data.waterColor }
		, mSunColour{ data.sunColor }
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
		glBufferData(GL_UNIFORM_BUFFER, 6 * 4 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 3, mBUF);
		updateGPU(true);
	}
	void renderUI();
	bool updateGPU(bool force);

private:
	BUF mBUF;

	ShaderGUIPair<glm::vec3> mDirtColour;
	ShaderGUIPair<glm::vec3> mMountainColour;
	ShaderGUIPair<glm::vec3> mGrassColour;
	ShaderGUIPair<glm::vec3> mSnowColour;
	ShaderGUIPair<glm::vec3> mWaterColour;
	ShaderGUIPair<glm::vec3> mSunColour;
};

#endif
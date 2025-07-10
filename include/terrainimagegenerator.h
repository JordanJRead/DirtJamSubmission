#ifndef TERRAIN_H
#define TERRAIN_H

#include "OpenGLObjects/FBO.h"
#include "OpenGLObjects/TEX.h"
#include "glm/glm.hpp"

class VertexArray;
class Shader;

class TerrainImageGenerator {
public:
	TerrainImageGenerator(int pixelDim, float worldSize, int screenWidth, int screenHeight, const glm::vec3& worldPos);
	void bindImage(int unit);

	int getPixelDim() { return mPixelDim; }
	float getWorldSize() { return mWorldSize; }
	const glm::vec3& getWorldPos() { return mWorldPos; }

private:
	FBO mFBO;
	TEX mColorTex;

	int mPixelDim;
	float mWorldSize;

	int mScreenWidth;
	int mScreenHeight;
	glm::vec3 mWorldPos;

private:
	void updatePixelDim(int pixelDim);
	void updateWorldSize(float worldSize);
	void updateWorldPos(const glm::vec3& worldPos);
	void updateTexture(const VertexArray& screenQuad, const Shader& terrainImageShader);
};

#endif
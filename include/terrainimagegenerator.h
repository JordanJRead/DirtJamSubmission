#ifndef TERRAIN_IMAGE_GENERATOR_H
#define TERRAIN_IMAGE_GENERATOR_H

#include "OpenGLObjects/FBO.h"
#include "OpenGLObjects/TEX.h"
#include "shader.h"
#include "vertexarray.h"
#include "glm/glm.hpp"

class TerrainImageGenerator {
public:
	TerrainImageGenerator(int pixelDim, int screenWidth, int screenHeight);
	void generateTexture(const VertexArray& screenVertexArray, const Shader& terrainNoiseShader, const glm::vec2& worldPos);
	void bindImage(int unit);
	void resetPixelDim(int pixelDim);

private:
	FBO mFBO;
	TEX mColorTex;

	int mPixelDim;

	int mScreenWidth;
	int mScreenHeight;

private:
};

#endif
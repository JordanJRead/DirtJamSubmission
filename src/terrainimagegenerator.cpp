#include "terrainimagegenerator.h"
#include <vector>
#include <iostream>
#include "vertexarray.h"
#include "shader.h"

TerrainImageGenerator::TerrainImageGenerator(int pixelDim, int screenWidth, int screenHeight)
	: mScreenWidth{ screenWidth }
	, mScreenHeight{ screenHeight }
	, mPixelDim{ pixelDim }
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	glBindTexture(GL_TEXTURE_2D, mColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mPixelDim, mPixelDim, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TerrainImageGenerator::resetPixelDim(int pixelDim) {
	mPixelDim = pixelDim;
	glBindTexture(GL_TEXTURE_2D, mColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mPixelDim, mPixelDim, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void TerrainImageGenerator::bindImage(int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	mColorTex.use(GL_TEXTURE_2D);
}

void TerrainImageGenerator::generateTexture(const VertexArray& screenVertexArray, const Shader& terrainNoiseShader, const glm::vec2& worldPos) {
	glViewport(0, 0, mPixelDim, mPixelDim);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	screenVertexArray.use();
	terrainNoiseShader.use();
	glDrawElements(GL_TRIANGLES, screenVertexArray.getIndexCount(), GL_UNSIGNED_INT, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, mScreenWidth, mScreenHeight);
}
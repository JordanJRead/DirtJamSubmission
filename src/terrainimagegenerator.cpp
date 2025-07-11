#include "terrainimagegenerator.h"
#include <vector>
#include <iostream>
#include "vertexarray.h"
#include "shader.h"

TerrainImageGenerator::TerrainImageGenerator(int pixelDim, float worldSize, int screenWidth, int screenHeight, const glm::vec3& worldPos)
	: mWorldSize{ worldSize }
	, mScreenWidth{ screenWidth }
	, mScreenHeight{ screenHeight }
	, mPixelDim{ pixelDim }
	, mWorldPos{ worldPos }
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

// Clears image
void TerrainImageGenerator::updatePixelDim(int pixelDim) {
	mPixelDim = pixelDim;
	glBindTexture(GL_TEXTURE_2D, mColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mPixelDim, mPixelDim, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void TerrainImageGenerator::bindImage(int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	mColorTex.use(GL_TEXTURE_2D);
}

void TerrainImageGenerator::setWorldSize(float worldSize) {
	mWorldSize = worldSize;
}

void TerrainImageGenerator::setWorldPos(const glm::vec3& worldPos) {
	mWorldPos = worldPos;
}

void TerrainImageGenerator::updateTexture(const VertexArray& screenQuad, const Shader& terrainImageShader) {
	glViewport(0, 0, mPixelDim, mPixelDim);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	screenQuad.use();
	terrainImageShader.use();

	terrainImageShader.setFloat("scale", mWorldSize);
	terrainImageShader.setVector3("worldPos", mWorldPos);

	glDrawElements(GL_TRIANGLES, screenQuad.getIndexCount(), GL_UNSIGNED_INT, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, mScreenWidth, mScreenHeight);
}
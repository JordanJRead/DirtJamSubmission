#include "terrainnoise.h"
#include <vector>
#include <iostream>

Terrain::Terrain(int pixelDim, float scale, int screenWidth, int screenHeight)
	: mPixelDim{ pixelDim }
	, mScale{ scale }
	, mPrevPixelDim{ -1 }
	, mPrevScale{ -1 }
	, mTerrainNoiseShader{ "shaders/terrainnoise.vert", "shaders/terrainnoise.frag" }
	, mScreenWidth{ screenWidth }
	, mScreenHeight{ screenHeight }
{
	std::vector<float> vertexData{
		-1, -1,
		 1, -1,
		-1,  1,
		 1,  1
	};

	std::vector<unsigned int> indices{
		0, 1, 2, 1, 2, 3
	};

	std::vector<int> attribs{
		2
	};

	mVertexArray.create(vertexData, indices, attribs);

	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	glBindTexture(GL_TEXTURE_2D, mColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mPixelDim, mPixelDim, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTex, 0);

	updateGPU();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Terrain::updatePixelDim() {
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	glBindTexture(GL_TEXTURE_2D, mColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mPixelDim, mPixelDim, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void Terrain::updateScale() {
	mTerrainNoiseShader.use();
	mTerrainNoiseShader.setFloat("scale", mScale);
}

void Terrain::updateGPU() {
	//bool hasChanged{ false };
	bool hasChanged{ true };

	if (mPrevPixelDim != mPixelDim) {
		updatePixelDim();
		mPrevPixelDim = mPixelDim;
		hasChanged = true;
	}

	if (mPrevScale != mScale) {
		updateScale();
		mPrevScale = mScale;
		hasChanged = true;
	}

	if (hasChanged) {
		updateTexture();
	}
}

void Terrain::updateTexture() {
	glViewport(0, 0, mPixelDim, mPixelDim);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mVertexArray.use();
	mTerrainNoiseShader.use();
	glDrawElements(GL_TRIANGLES, mVertexArray.getIndexCount(), GL_UNSIGNED_INT, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, mScreenWidth, mScreenHeight);
}
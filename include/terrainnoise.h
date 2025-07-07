#ifndef TERRAIN_H
#define TERRAIN_H

#include "OpenGLObjects/FBO.h"
#include "OpenGLObjects/TEX.h"
#include "shader.h"
#include "vertexarray.h"

class Terrain {
public:
	Terrain(int pixelDim, float scale, int screenWidth, int screenHeight);
	void updateGPU(bool forceUpdate = false);
	void bindImage(int unit);

	int* getPixelDimPtr() { return &mPixelDim; }
	float* getScalePtr() { return &mScale; }

private:
	FBO mFBO;
	TEX mColorTex;
	Shader mTerrainNoiseShader;
	VertexArray mVertexArray;

	int mPixelDim;
	float mScale;

	int mPrevPixelDim;
	float mPrevScale;

	int mScreenWidth;
	int mScreenHeight;

private:
	void updatePixelDim();
	void updateScale();
	void updateTexture();
};

#endif
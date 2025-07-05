#ifndef TERRAIN_H
#define TERRAIN_H

#include "OpenGLObjects/FBO.h"
#include "OpenGLObjects/TEX.h"

class Terrain {
public:
	Terrain(int dim, float scale);
	int getDim() { return mDim; }
	float getScale() { return mScale; }

private:
	FBO mFBO;
	TEX mColorTex;
	int mDim;
	float mScale;
};

#endif
#ifndef TERRAIN_RENDERER_H
#define TERRAIN_RENDERER_H

#include "glm/glm.hpp"
#include "vertexarray.h"
#include "terrainimagegenerator.h"

class TerrainRenderer {
public:
	void render(const glm::vec3& cameraPos);
	glm::vec3 getClosestWorldPixelPos(const glm::vec3 pos);

private:
	// The chunk collection consists of a square of smallChunkCount * smallChunkCount chunks, each having a width of smallChunkWidth
	// Then, there will 8 chunks place around this square, each one having a width of smallChunkCount * smallChunkWidth (the width of this small chunk square)
	// This is repeated with increasingly sized large chunks (3x each time) for largeChunkRingCount times

	// The small chunks will go from high to low quality, while the far chunks will all be low quality?
	int mSmallChunkWidth;
	int mSmallChunkCount;
	int mLargeChunkRingCount;
	VertexArray mLowQualityPlane; // Maybe make plane objects?
	VertexArray mMedQualityPlane;
	VertexArray mHighQualityPlane;
	VertexArray mScreenQuad;

	/*
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
	*/

	TerrainImageGenerator mLowQualityImage;
	TerrainImageGenerator mMedQualityImage;
	TerrainImageGenerator mHighQualityImage;
};

#endif
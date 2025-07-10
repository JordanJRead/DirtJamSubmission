#include "chunkmanager.h"
#include "glm/glm.hpp"
#include <vector>

ChunkManager::ChunkManager(int chunkWidth, int chunkDimCount, int screenWidth, int screenHeight)
	: mChunkWidth{ chunkWidth }
	, mCenterChunkPosition{ glm::vec2 {0, 0}}
	, mTerrainImageShader{ "shaders/terrainnoise.vert", "shaders/terriannoise.frag" }
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

	mScreenVertexArray.create(vertexData, indices, attribs);

	mTerrainImageShader.use();
	mTerrainImageShader.setFloat("scale", chunkWidth);

	int middleIndex{ chunkDimCount / 2 };

	for (int i{ 0 }; i < chunkDimCount; ++i) {
		mImages.emplace_back();
		for (int j{ 0 }; j < chunkDimCount; ++j) {
			int xChunkOffset{ j - middleIndex };
			int yChunkOffset{ i - middleIndex };
			float xOffset = xChunkOffset * mChunkWidth;
			float yOffset = yChunkOffset * mChunkWidth;
			glm::vec2 chunkPos{ mCenterChunkPosition + glm::vec2 { xOffset, yOffset } };

			float dist{ glm::length(mCenterChunkPosition - chunkPos) };

			mImages[i].emplace_back(distToPixelQuality(dist), screenWidth, screenHeight);
			mImages[i][j].generateTexture(mScreenVertexArray, mTerrainImageShader, chunkPos);
		}
	}
}

float ChunkManager::distToPixelQuality(float dist) {
	return 1024;
}
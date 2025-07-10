#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include "glm/glm.hpp"
#include <vector>
#include "vertexarray.h"
#include "shader.h"
#include "terrainimagegenerator.h"

class ChunkManager {
public:
	ChunkManager(int chunkWidth, int chunkDimCount, int screenWidth, int screenHeight);

private:
	float mChunkWidth;
	glm::vec2 mCenterChunkPosition;
	VertexArray mScreenVertexArray;
	Shader mTerrainImageShader;
	std::vector<std::vector<TerrainImageGenerator>> mImages;

private:
	float distToPixelQuality(float dist);
};
/*
PER FRAME:
	Update
		If player moved:
			shift over all FBOs and textures
				End deletes it's stuff
				Shift everything over
				Generate new image for other end
		Pixel dim
			check each chunk
			if it's distance does not correlate to it's pixel quality, update its quality and rerender
	Render
		If the chunk is not behind the player:
			render it
*/
#endif
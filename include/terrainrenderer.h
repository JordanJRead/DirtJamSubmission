#ifndef TERRAIN_RENDERER_H
#define TERRAIN_RENDERER_H

#include "shader.h"
#include "glm/glm.hpp"
#include "vertexarray.h"
#include "terrainimagegenerator.h"
#include "plane.h"
#include <array>
#include <string>
#include "artisticparamsbuffer.h"
#include "imgui/imgui.h"

constexpr int ImageCount{ 3 };
//template <int ImageCount>

class TerrainRenderer {
public:
	TerrainRenderer(int screenWidth, int screenHeight, const glm::vec3& cameraPos,
		int smallChunkWidth, int smallCountCount, int largeChunkRingCount, const ArtisticParamsBuffer& artistParams,
		std::array<int, ImageCount> imagePixelDims, std::array<float, ImageCount> imageWorldSizes, std::array<glm::vec2, ImageCount> imageWorldPositions,
		int lowQualityPlaneVertexDensity, int medQualityPlaneVertexDensity, int highQualityPlaneVertexDensity)
		: mSmallChunkWidth{ smallChunkWidth }
		, mSmallChunkCount{ smallCountCount }
		, mLargeChunkRingCount{ largeChunkRingCount }
		
		, mArtisticParams{ artistParams }

		, mLowQualityPlaneVertexDensity{ lowQualityPlaneVertexDensity }
		, mMedQualityPlaneVertexDensity{ medQualityPlaneVertexDensity }
		, mHighQualityPlaneVertexDensity{ highQualityPlaneVertexDensity }

		, mLowQualityPlane{ mLowQualityPlaneVertexDensity }
		, mMedQualityPlane{ mMedQualityPlaneVertexDensity }
		, mHighQualityPlane{ mHighQualityPlaneVertexDensity }

		, mTerrainImageShader{ "shaders/terrainnoise.vert", "shaders/terrainnoise.frag" }
		, mTerrainShader{ "shaders/terrainnoise.vert", "shaders/terrainnoise.frag" }

		, mImageWorldPositions{ imageWorldPositions }
		, mImagePixelDims{ imagePixelDims }
		, mImageWorldSizes{ imageWorldSizes }

		, mImages{ {
			{mImagePixelDims[0], mImageWorldSizes[0], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos)},
			{mImagePixelDims[1], mImageWorldSizes[1], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos)},
			{mImagePixelDims[2], mImageWorldSizes[2], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos)}
		} }
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

		mScreenQuad.create(vertexData, indices, attribs);

		// Set shader uniforms
		mTerrainShader.use();
		mTerrainShader.setInt("imageCount", ImageCount);
		for (int i{ 0 }; i < ImageCount; ++i) {
			std::string indexString{ std::to_string(i) };
			mTerrainShader.setInt("images[" + indexString + "]", i);
			mTerrainShader.setFloat("imageScales[" + indexString + "]", mImageWorldSizes[i]);
			mTerrainShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);
		}
	}

	void render(const glm::vec3& cameraPos) {
		mTerrainShader.use();

		// Update plane types
		if (mLowQualityPlaneVertexDensity != mLowQualityPlane.getVerticesPerEdge()) {
			mLowQualityPlane.rebuild(mLowQualityPlaneVertexDensity);
		}
		if (mMedQualityPlaneVertexDensity != mMedQualityPlane.getVerticesPerEdge()) {
			mMedQualityPlane.rebuild(mMedQualityPlaneVertexDensity);
		}
		if (mHighQualityPlaneVertexDensity != mHighQualityPlane.getVerticesPerEdge()) {
			mHighQualityPlane.rebuild(mHighQualityPlaneVertexDensity);
		}

		// Update images
		for (int i{ 0 }; i < ImageCount; ++i) {

			// TODO find out if the images should move, and to where

			std::string indexString{ std::to_string(i) };
			bool hasImageChanged{ false };

			if (mImages[i].getWorldSize() != mImageWorldSizes[i]) {
				mImages[i].setWorldSize(mImageWorldSizes[i]);
				mTerrainShader.setFloat("imageScales[" + indexString + "]", mImageWorldSizes[i]);
				hasImageChanged = true;
			}

			if (mImages[i].getPixelDim() != mImagePixelDims[i]) {
				mImages[i].updatePixelDim(mImagePixelDims[i]);
				hasImageChanged = true;
			}

			if (mImages[i].getWorldPos() != mImageWorldPositions[i]) { // Updated automatically
				mImages[i].setWorldPos(mImageWorldPositions[i]);
				mTerrainShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);
				hasImageChanged = true;
			}

			if (hasImageChanged) {
				mImages[i].updateTexture(mScreenQuad, mTerrainImageShader); // binds another shader
			}
		}

		mTerrainShader.use();
		
	}
	void renderUI() {
		mArtisticParams.renderUI();

		ImGui::Begin("Plane Chunking");
		ImGui::End();

		ImGui::Begin("Terrain Images");
		for (int i{ 0 }; i < ImageCount; ++i) {
			ImGui::DragFloat("World size", &mImageWorldSizes[i], 1, 1, 100000);
			ImGui::InputInt("Pixel quality", &mImagePixelDims[i], 100, 1000);
		}
		ImGui::End();
	}

	glm::vec3 getClosestWorldPixelPos(const glm::vec3 pos);

private:
	// The chunk collection consists of a square of smallChunkCount * smallChunkCount chunks, each having a width of smallChunkWidth
	// Then, there will 8 chunks place around this square, each one having a width of smallChunkCount * smallChunkWidth (the width of this small chunk square)
	// This is repeated with increasingly sized large chunks (3x each time) for largeChunkRingCount times

	// The small chunks will go from high to low quality, while the far chunks will all be low quality?
	int mSmallChunkWidth;
	int mSmallChunkCount;
	int mLargeChunkRingCount;

	ArtisticParamsBuffer mArtisticParams;

	std::array<int, ImageCount> mImagePixelDims;
	std::array<float, ImageCount> mImageWorldSizes;
	std::array<glm::vec2, ImageCount> mImageWorldPositions;
	std::array<TerrainImageGenerator, ImageCount> mImages;

	int mLowQualityPlaneVertexDensity;
	int mMedQualityPlaneVertexDensity;
	int mHighQualityPlaneVertexDensity;

	Shader mTerrainImageShader;
	Shader mTerrainShader;

	Plane mLowQualityPlane;
	Plane mMedQualityPlane;
	Plane mHighQualityPlane;

	VertexArray mScreenQuad;
};

#endif
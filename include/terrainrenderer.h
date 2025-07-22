#ifndef TERRAIN_RENDERER_H
#define TERRAIN_RENDERER_H

#include "shader.h"
#include "glm/glm.hpp"
#include "vertexarray.h"
#include "terrainimagegenerator.h"
#include "plane.h"
#include <array>
#include <string>
#include <string_view>
#include "artisticparamsbuffer.h"
#include "terrainparamsbuffer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "camera.h"
#include <iostream>

constexpr int ImageCount{ 3 };
//template <int ImageCount>

class TerrainRenderer {
public:
	TerrainRenderer(int screenWidth, int screenHeight, const glm::vec3& cameraPos,
		int smallChunkWidth, int smallChunkCount, int largeChunkRingCount, const ArtisticParamsData& artistParams, const TerrainParamsData& terrainParams,
		std::array<int, ImageCount> imagePixelDims, std::array<float, ImageCount> imageWorldSizes, std::array<glm::vec2, ImageCount> imageWorldPositions,
		int lowQualityPlaneVerticesPerEdge, int medQualityPlaneVerticesPerEdgeScale, int highQualityPlaneVerticesPerEdgeScale)
		: mSmallChunkWidth{ smallChunkWidth }
		, mSmallChunkCount{ smallChunkCount }
		, mLargeChunkRingCount{ largeChunkRingCount }
		
		, mArtisticParams{ artistParams }
		, mTerrainParams{ terrainParams }

		, mLowQualityPlaneVerticesPerEdge{ lowQualityPlaneVerticesPerEdge }
		, mMedQualityPlaneVerticesPerEdgeScale{ medQualityPlaneVerticesPerEdgeScale }
		, mHighQualityPlaneVerticesPerEdgeScale{ highQualityPlaneVerticesPerEdgeScale }

		, mLowQualityPlane{ mLowQualityPlaneVerticesPerEdge }
		, mMedQualityPlane{ mMedQualityPlaneVerticesPerEdgeScale }
		, mHighQualityPlane{ mHighQualityPlaneVerticesPerEdgeScale }

		, mTerrainImageShader{ "shaders/terrainimage.vert", "shaders/terrainimage.frag" }
		, mTerrainShader{ "shaders/terrain.vert", "shaders/terrain.frag" }

		, mImageWorldPositions{ imageWorldPositions }
		, mImagePixelDims{ imagePixelDims }
		, mImageWorldSizes{ imageWorldSizes }

		, mImages{ {
			{mImagePixelDims[0], mImageWorldSizes[0], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 0)},
			{mImagePixelDims[1], mImageWorldSizes[1], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 1)},
			{mImagePixelDims[2], mImageWorldSizes[2], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 2)}
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
			mImages[i].updateTexture(mScreenQuad, mTerrainImageShader);
			mTerrainShader.use();
		}
	}

	void render(const Camera& camera) {
		bool hasTerrainChanged{ mTerrainParams.updateGPU(false) };
		mArtisticParams.updateGPU(false);
		mTerrainShader.use();

		// Update plane types
		if (mLowQualityPlaneVerticesPerEdge != mLowQualityPlane.getVerticesPerEdge()) {
			mLowQualityPlane.rebuild(mLowQualityPlaneVerticesPerEdge);
		}
		if (mMedQualityPlaneVerticesPerEdgeScale * mLowQualityPlaneVerticesPerEdge != mMedQualityPlane.getVerticesPerEdge()) {
			mMedQualityPlane.rebuild(mMedQualityPlaneVerticesPerEdgeScale * mLowQualityPlaneVerticesPerEdge);
		}
		if (mHighQualityPlaneVerticesPerEdgeScale * mLowQualityPlaneVerticesPerEdge != mHighQualityPlane.getVerticesPerEdge()) {
			mHighQualityPlane.rebuild(mHighQualityPlaneVerticesPerEdgeScale * mLowQualityPlaneVerticesPerEdge);
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

			if (hasImageChanged || hasTerrainChanged) {
				mImages[i].updateTexture(mScreenQuad, mTerrainImageShader); // binds another shader
				mTerrainShader.use();
			}
		}

		mTerrainShader.use();
		mTerrainShader.setMatrix4("view", camera.getViewMatrix());
		mTerrainShader.setMatrix4("proj", camera.getProjectionMatrix());

		for (int i{ 0 }; i < mImages.size(); ++i) {
			mImages[i].bindImage(i);
		}

 		for (int x{ -mSmallChunkCount / 2 }; x <= mSmallChunkCount / 2; ++x) {
			for (int z{ -mSmallChunkCount / 2 }; z <= mSmallChunkCount / 2; ++z) {

				glm::vec3 smoothChunkPos{ camera.getPosition() - glm::vec3(x * mSmallChunkWidth, 0, z * mSmallChunkWidth) };
				float chunkDist{ glm::length(smoothChunkPos - camera.getPosition()) };
				Plane* currPlane;
				if (chunkDist > 500) {
					currPlane = &mLowQualityPlane;
				}
				else if (chunkDist > 100) {
					currPlane = &mMedQualityPlane;
				}
				else {
					currPlane = &mHighQualityPlane;
				}

				glm::vec3 chunkPos{ getClosestWorldVertexPos(camera.getPosition()) - glm::vec3(x * mSmallChunkWidth, 0, z * mSmallChunkWidth)};
				mTerrainShader.setVector3("planePos", { chunkPos.x, 0, chunkPos.z });
				mTerrainShader.setFloat("planeWorldWidth", mSmallChunkWidth);

				currPlane->useVertexArray();
				for (int i{ 0 }; i < mArtisticParams.getMaxShellCount(); ++i) {
					mTerrainShader.setInt("shellIndex", i);
					glDrawElements(GL_TRIANGLES, currPlane->getIndexCount(), GL_UNSIGNED_INT, 0);
				}
			}
		}

		renderUI();
	}

	glm::vec3 getClosestWorldPixelPos(const glm::vec3 pos, int imageIndex) {
		float stepSize{ mImageWorldSizes[imageIndex] / mImagePixelDims[imageIndex] * 22 };
		glm::vec3 stepSizesAway = pos / stepSize;
		stepSizesAway = glm::vec3{ (int)stepSizesAway.x, (int)stepSizesAway.y, (int)stepSizesAway.z };
		return stepSizesAway * stepSize;
	}

	glm::vec3 getClosestWorldVertexPos(const glm::vec3 pos) {
		float stepSize{ mLowQualityPlane.getStepSize() * mSmallChunkWidth };
		glm::vec3 stepSizesAway = pos / stepSize;
		stepSizesAway = glm::vec3{ (int)stepSizesAway.x, (int)stepSizesAway.y, (int)stepSizesAway.z };
		return stepSizesAway * stepSize;
	}

private:
	// The chunk collection consists of a square of smallChunkCount * smallChunkCount chunks, each having a width of smallChunkWidth
	// Then, there will 8 chunks place around this square, each one having a width of smallChunkCount * smallChunkWidth (the width of this small chunk square)
	// This is repeated with increasingly sized large chunks (3x each time) for largeChunkRingCount times

	// The small chunks will go from high to low quality, while the far chunks will all be low quality?
	int mSmallChunkWidth;
	int mSmallChunkCount;
	int mLargeChunkRingCount;

	ArtisticParamsBuffer mArtisticParams;
	TerrainParamsBuffer mTerrainParams;

	std::array<int, ImageCount> mImagePixelDims;
	std::array<float, ImageCount> mImageWorldSizes;
	std::array<glm::vec2, ImageCount> mImageWorldPositions;
	std::array<TerrainImageGenerator, ImageCount> mImages;

	int mLowQualityPlaneVerticesPerEdge;
	int mMedQualityPlaneVerticesPerEdgeScale;
	int mHighQualityPlaneVerticesPerEdgeScale;

	Shader mTerrainImageShader;
	Shader mTerrainShader;

	Plane mLowQualityPlane;
	Plane mMedQualityPlane;
	Plane mHighQualityPlane;

	VertexArray mScreenQuad;

	void renderUI() {

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		mArtisticParams.renderUI();
		mTerrainParams.renderUI();

		ImGui::Begin("Plane Chunking");
		ImGui::DragInt("Width", &mSmallChunkWidth, 1, 1, 100);
		ImGui::DragInt("Count", &mSmallChunkCount, 1, 1, 100);
		ImGui::DragInt("Low quality plane vertices", &mLowQualityPlaneVerticesPerEdge, 1, 2, 1000);
		ImGui::DragInt("Med quality plane quality scale", &mMedQualityPlaneVerticesPerEdgeScale, 1, 2, 1000);
		ImGui::DragInt("High quality plane quality scale", &mHighQualityPlaneVerticesPerEdgeScale, 1, 2, 1000);
		ImGui::End();

		ImGui::Begin("Terrain Images");
		for (int i{ 0 }; i < ImageCount; ++i) {
			std::string indexString{ std::to_string(i + 1) };
			ImGui::DragFloat(("World size " + indexString).c_str(), &mImageWorldSizes[i], 1, 1, 100000);
			ImGui::InputInt(("Pixel quality " + indexString).c_str(), &mImagePixelDims[i], 100, 1000);
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};

#endif
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

constexpr int ImageCount{ 3 };
//template <int ImageCount>

class TerrainRenderer {
public:
	TerrainRenderer(int screenWidth, int screenHeight, const glm::vec3& cameraPos,
		int smallChunkWidth, int smallChunkCount, int largeChunkRingCount, const ArtisticParamsData& artistParams, const TerrainParamsData& terrainParams,
		std::array<int, ImageCount> imagePixelDims, std::array<float, ImageCount> imageWorldSizes, std::array<glm::vec2, ImageCount> imageWorldPositions,
		int lowQualityPlaneVertexDensity, int medQualityPlaneVertexDensity, int highQualityPlaneVertexDensity)
		: mSmallChunkWidth{ smallChunkWidth }
		, mSmallChunkCount{ smallChunkCount }
		, mLargeChunkRingCount{ largeChunkRingCount }
		
		, mArtisticParams{ artistParams }
		, mTerrainParams{ terrainParams }

		, mLowQualityPlaneVerticesPerEdge{ lowQualityPlaneVertexDensity }
		, mMedQualityPlaneVerticesPerEdge{ medQualityPlaneVertexDensity }
		, mHighQualityPlaneVerticesPerEdge{ highQualityPlaneVertexDensity }

		, mLowQualityPlane{ mLowQualityPlaneVerticesPerEdge }
		, mMedQualityPlane{ mMedQualityPlaneVerticesPerEdge }
		, mHighQualityPlane{ mHighQualityPlaneVerticesPerEdge }

		, mTerrainImageShader{ "shaders/terrainimage.vert", "shaders/terrainimage.frag" }
		, mTerrainShader{ "shaders/terrain.vert", "shaders/terrain.frag" }

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

	void render(const Camera& camera) {
		renderUI();
		mTerrainShader.use();

		// Update plane types
		if (mLowQualityPlaneVerticesPerEdge != mLowQualityPlane.getVerticesPerEdge()) {
			mLowQualityPlane.rebuild(mLowQualityPlaneVerticesPerEdge);
		}
		if (mMedQualityPlaneVerticesPerEdge != mMedQualityPlane.getVerticesPerEdge()) {
			mMedQualityPlane.rebuild(mMedQualityPlaneVerticesPerEdge);
		}
		if (mHighQualityPlaneVerticesPerEdge != mHighQualityPlane.getVerticesPerEdge()) {
			mHighQualityPlane.rebuild(mHighQualityPlaneVerticesPerEdge);
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
		mTerrainShader.setMatrix4("view", camera.getViewMatrix());
		mTerrainShader.setMatrix4("proj", camera.getProjectionMatrix());

		for (int i{ 0 }; i < mImages.size(); ++i) {
			mImages[i].bindImage(i);
		}
		// foreach plane
		mTerrainShader.setVector3("planePos", camera.getPosition());
		mTerrainShader.setFloat("planeWorldWidth", mSmallChunkWidth);
		for (int i{ 0 }; i < mArtisticParams.getMaxShellCount(); ++i) {
			mTerrainShader.setInt("shellIndex", i);
			mHighQualityPlane.useVertexArray();
			glDrawElements(GL_TRIANGLES, mHighQualityPlane.getIndexCount(), GL_UNSIGNED_INT, 0);
		}
	}

	glm::vec3 getClosestWorldPixelPos(const glm::vec3 pos) {
		return pos;
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
	int mMedQualityPlaneVerticesPerEdge;
	int mHighQualityPlaneVerticesPerEdge;

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
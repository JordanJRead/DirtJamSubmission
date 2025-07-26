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
#include "cpugpupair.h"
#include "intssbo.h"

constexpr int ImageCount{ 3 };
//template <int ImageCount>

class TerrainRenderer {
public:
	TerrainRenderer(int screenWidth, int screenHeight, const glm::vec3& cameraPos,
		int chunkWidth, int chunkCount, const ArtisticParamsData& artistParams, const TerrainParamsData& terrainParams,
		std::array<int, ImageCount> imagePixelDims, std::array<float, ImageCount> imageWorldSizes, std::array<glm::vec2, ImageCount> imageWorldPositions,
		int lowQualityPlaneVerticesPerEdge, int highQualityPlaneVerticesPerEdgeScale, float vertexQualityDropoffDistance, float waterHeight)
		: mChunkWidth{ chunkWidth }
		, mChunkCount{ chunkCount }
		
		, mArtisticParams{ artistParams }
		, mTerrainParams{ terrainParams }

		, mLowQualityPlaneVerticesPerEdge{ lowQualityPlaneVerticesPerEdge }
		, mHighQualityPlaneVerticesPerEdgeScale{ highQualityPlaneVerticesPerEdgeScale }

		, mLowQualityPlane{ mLowQualityPlaneVerticesPerEdge }
		, mHighQualityPlane{ mHighQualityPlaneVerticesPerEdgeScale }

		, mTerrainImageShader{ "shaders/terrainimage.vert", "shaders/terrainimage.frag" }
		, mTerrainShader{ "shaders/terrain.vert", "shaders/terrain.frag" }
		, mWaterShader{ "shaders/water.vert", "shaders/water.frag" }

		, mImageWorldPositions{ imageWorldPositions }
		, mImagePixelDims{ imagePixelDims }
		, mImageWorldSizes{ imageWorldSizes }

		, mVertexQualityDropoffDistance{ vertexQualityDropoffDistance }

		, mImages{ {
			{mImagePixelDims[0], mImageWorldSizes[0], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 0)},
			{mImagePixelDims[1], mImageWorldSizes[1], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 1)},
			{mImagePixelDims[2], mImageWorldSizes[2], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 2)}
		} }

		, mWaterHeight{ waterHeight }
		, mLowQualityPlaneStepSize{ -1 }
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

	void render(const Camera& camera, double displayDeltaTime) {
		bool hasTerrainChanged{ mTerrainParams.updateGPU(false) };
		mArtisticParams.updateGPU(false);
		mTerrainShader.use();

		// Update plane types
		if (mLowQualityPlaneVerticesPerEdge != mLowQualityPlane.getVerticesPerEdge()) {
			mLowQualityPlane.rebuild(mLowQualityPlaneVerticesPerEdge);
		}

		int highQualityVerticesPerEdge{ mHighQualityPlaneVerticesPerEdgeScale * (mLowQualityPlaneVerticesPerEdge - 1) + 1 }; // We want the distance between vertices to be multiples of each other, so we do this
		if (highQualityVerticesPerEdge != mHighQualityPlane.getVerticesPerEdge()) {
			mHighQualityPlane.rebuild(highQualityVerticesPerEdge);
		}

		// Update images
		for (int i{ 0 }; i < ImageCount; ++i) {

			// Move images along with the player
			glm::vec2 scaledCameraPos{ glm::vec2(camera.getPosition().x, camera.getPosition().z) / mArtisticParams.getTerrainScale() };
			double cameraDistFromImageCenter{ glm::length(scaledCameraPos - mImageWorldPositions[i]) };
			if (cameraDistFromImageCenter * 2 > 0.5 * mImageWorldSizes[i]) { // If near edge of image, update image
				glm::vec3 pixelPosition{ getClosestWorldPixelPos(camera.getPosition() / mArtisticParams.getTerrainScale(), i)};
				mImageWorldPositions[i] = glm::vec2(pixelPosition.x, pixelPosition.z);
			}

			// Recalculate image checks
			std::string indexString{ std::to_string(i) };
			bool hasImageChanged{ false };

			// GUI
			if (mImages[i].getWorldSize() != mImageWorldSizes[i]) {
				mImages[i].setWorldSize(mImageWorldSizes[i]);
				mTerrainShader.setFloat("imageScales[" + indexString + "]", mImageWorldSizes[i]);
				hasImageChanged = true;
			}

			// GUI
			if (mImages[i].getPixelDim() != mImagePixelDims[i]) {
				mImages[i].updatePixelDim(mImagePixelDims[i]);
				hasImageChanged = true;
			}

			// The above position calculation
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

		// Set uniforms
		mTerrainShader.use();
		mTerrainShader.setMatrix4("view", camera.getViewMatrix());
		mTerrainShader.setMatrix4("proj", camera.getProjectionMatrix());
		mTerrainShader.setVector3("cameraPos", camera.getPosition());
		if (mChunkWidth.hasDiff() || mIsFirstFrame) {
			mChunkWidth.mGPU = mChunkWidth.mCPU;
			mTerrainShader.setInt("chunkWidth", mChunkWidth.mCPU);
		}
		mLowQualityPlaneStepSize.mCPU = mLowQualityPlane.getStepSize();
		if (mLowQualityPlaneStepSize.hasDiff() || mIsFirstFrame) {
			mLowQualityPlaneStepSize.mGPU = mLowQualityPlaneStepSize.mCPU;
			mTerrainShader.setFloat("lowQualityPlaneStepSize", mLowQualityPlaneStepSize.mCPU);
		}
		mWaterShader.use();
		mWaterShader.setMatrix4("view", camera.getViewMatrix());
		mWaterShader.setMatrix4("proj", camera.getProjectionMatrix());

		for (int i{ 0 }; i < mImages.size(); ++i) {
			mImages[i].bindImage(i);
		}

		int oddChunkCount{ (mChunkCount.mCPU / 2) * 2 + 1 };
		// Update chunk SSBOs
		bool updatePositions{ mChunkCount.hasDiff() || mIsFirstFrame };

		if (updatePositions)
			mChunkCount.mGPU = mChunkCount.mCPU; // GPU means stored value, CPU means GUI value

		std::vector<int> positions;
		std::vector<int> visibleHighQualityChunkIndices;
		std::vector<int> visibleLowQualityChunkIndices;

		if (updatePositions)
		positions.reserve(oddChunkCount * oddChunkCount * 2);
		int i{ 0 };
		for (int x{ -oddChunkCount / 2 }; x <= oddChunkCount / 2; ++x) {
			for (int z{ -oddChunkCount / 2 }; z <= oddChunkCount / 2; ++z) {
				glm::vec3 chunkPos{ camera.getPosition() - glm::vec3(x * mChunkWidth.mCPU, 0, z * mChunkWidth.mCPU) };
				float chunkDist{ glm::length(chunkPos - camera.getPosition()) };
				bool isHighQuality{ chunkDist < mVertexQualityDropoffDistance };
				if (updatePositions) {
					positions.push_back(x);
					positions.push_back(z);
				}

				if (isHighQuality) {
					visibleHighQualityChunkIndices.push_back(i);
				}
				else {
					visibleLowQualityChunkIndices.push_back(i);
				}
				++i;
			}
		}

		if (updatePositions)
			mSSBOChunkPositions.UploadData(positions, GL_STATIC_DRAW);

		// Draw
		mTerrainShader.use();
		int shellCount{ mArtisticParams.getShellCount() };

		mSSBOChunkIndices.UploadData(visibleHighQualityChunkIndices, GL_STREAM_DRAW);
		mHighQualityPlane.useVertexArray();
		glDrawElementsInstanced(GL_TRIANGLES, mHighQualityPlane.getIndexCount(), GL_UNSIGNED_INT, 0, (shellCount + 1) * visibleHighQualityChunkIndices.size()); // Draw each shell plus the base terrain

		mSSBOChunkIndices.UploadData(visibleLowQualityChunkIndices, GL_STREAM_DRAW);
		mLowQualityPlane.useVertexArray();
		glDrawElementsInstanced(GL_TRIANGLES, mHighQualityPlane.getIndexCount(), GL_UNSIGNED_INT, 0, (shellCount + 1) * visibleLowQualityChunkIndices.size()); // Draw each shell plus the base terrain

		renderUI(displayDeltaTime);
		mIsFirstFrame = false;
	}

	glm::vec3 getClosestWorldPixelPos(const glm::vec3 pos, int imageIndex) {
		float stepSize{ mImageWorldSizes[imageIndex] / mImagePixelDims[imageIndex] * mArtisticParams.getTerrainScale() };
		glm::vec3 stepSizesAway = pos / stepSize;
		stepSizesAway = glm::vec3{ (int)stepSizesAway.x, (int)stepSizesAway.y, (int)stepSizesAway.z };
		return stepSizesAway * stepSize;
	}

private:
	// The chunk collection consists of a square of chunkCount * chunkCount chunks, each having a width of chunkWidth

	// The chunks will go from high to low quality, while the far chunks will all be low quality?
	CPUGPUPair<int> mChunkWidth;
	CPUGPUPair<float> mLowQualityPlaneStepSize;
	bool mIsFirstFrame{ true };
	CPUGPUPair<int> mChunkCount;

	IntSSBO mSSBOChunkPositions{ 0 };
	IntSSBO mSSBOChunkIndices{ 1 };

	ArtisticParamsBuffer mArtisticParams;
	TerrainParamsBuffer mTerrainParams;

	std::array<int, ImageCount> mImagePixelDims;
	std::array<float, ImageCount> mImageWorldSizes;
	std::array<glm::vec2, ImageCount> mImageWorldPositions;
	std::array<TerrainImageGenerator, ImageCount> mImages;

	int mLowQualityPlaneVerticesPerEdge;
	int mHighQualityPlaneVerticesPerEdgeScale;
	float mWaterHeight;

	Shader mTerrainImageShader;
	Shader mTerrainShader;
	Shader mWaterShader;

	Plane mLowQualityPlane;
	Plane mHighQualityPlane;

	float mVertexQualityDropoffDistance;

	VertexArray mScreenQuad;

	void renderUI(double displayDeltaTime) {

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("FPS");
		ImGui::LabelText(std::to_string(1 / displayDeltaTime).c_str(), "");
		ImGui::End();

		mArtisticParams.renderUI();
		mTerrainParams.renderUI();

		ImGui::Begin("Plane Chunking");
		ImGui::DragInt("Width", &mChunkWidth.mCPU, 1, 1, 100);
		ImGui::DragInt("Count", &mChunkCount.mCPU, 1, 1, 100);
		ImGui::DragInt("Low quality plane vertices", &mLowQualityPlaneVerticesPerEdge, 1, 2, 1000);
		ImGui::DragInt("High quality plane quality scale", &mHighQualityPlaneVerticesPerEdgeScale, 1, 2, 1000);
		ImGui::DragFloat("Vertex LOD dist", &mVertexQualityDropoffDistance, 1, 1, 1000);
		ImGui::DragFloat("Water height", &mWaterHeight, 0.1);
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
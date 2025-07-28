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
#include "waterparamsbuffer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "camera.h"
#include <iostream>

constexpr int ImageCount{ 4 };
//template <int ImageCount>

class TerrainRenderer {
public:
	TerrainRenderer(int screenWidth, int screenHeight, const glm::vec3& cameraPos,
		int chunkWidth, int chunkCount, const ArtisticParamsData& artistParams, const TerrainParamsData& terrainParams, const WaterParamsData& waterParams,
		std::array<int, ImageCount> imagePixelDims, std::array<float, ImageCount> imageWorldSizes, std::array<glm::vec2, ImageCount> imageWorldPositions,
		int lowQualityPlaneVerticesPerEdge, int highQualityPlaneVerticesPerEdgeScale, float vertexQualityDropoffDistance, float waterHeight)
		: mChunkWidth{ chunkWidth }
		, mChunkCount{ chunkCount }
		
		, mArtisticParams{ artistParams }
		, mTerrainParams{ terrainParams }
		, mWaterParams{ waterParams }

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
			{mImagePixelDims[2], mImageWorldSizes[2], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 2)},
			{mImagePixelDims[3], mImageWorldSizes[3], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 3)}
		} }

		, mWaterHeight{ waterHeight }
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

	void render(const Camera& camera, double displayDeltaTime, float time) {
		bool hasTerrainChanged{ mTerrainParams.updateGPU(false) };
		mArtisticParams.updateGPU(false);
		mWaterParams.updateGPU(false);
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
			if (cameraDistFromImageCenter * 2 > 0.2 * mImageWorldSizes[i]) { // If near edge of image, update image
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

		mTerrainShader.use();
		mTerrainShader.setMatrix4("view", camera.getViewMatrix());
		mTerrainShader.setMatrix4("proj", camera.getProjectionMatrix());
		mWaterShader.use();
		mWaterShader.setMatrix4("view", camera.getViewMatrix());
		mWaterShader.setMatrix4("proj", camera.getProjectionMatrix());
		mWaterShader.setFloat("time", time);

		for (int i{ 0 }; i < mImages.size(); ++i) {
			mImages[i].bindImage(i);
		}

		// For each chunk
 		for (int x{ -mChunkCount / 2 }; x <= mChunkCount / 2; ++x) {
			for (int z{ -mChunkCount / 2 }; z <= mChunkCount / 2; ++z) {

				mTerrainShader.use();
				glm::vec3 chunkPos{ getClosestWorldVertexPos(camera.getPosition()) - glm::vec3(x * mChunkWidth, 0, z * mChunkWidth) };
				float chunkDist{ glm::length(chunkPos - camera.getPosition()) };
				Plane& currPlane{ chunkDist > mVertexQualityDropoffDistance ? mLowQualityPlane : mHighQualityPlane };

				mTerrainShader.setVector3("planePos", { chunkPos.x, 0, chunkPos.z });
				mTerrainShader.setFloat("planeWorldWidth", mChunkWidth);

				currPlane.useVertexArray();

				int shellCount{ mArtisticParams.getShellCount() };

				// glInstanceID is 1 greater than the shellIndex (base terrain is -1 shell index, first shell is 0 shell index)
				glDrawElementsInstanced(GL_TRIANGLES, currPlane.getIndexCount(), GL_UNSIGNED_INT, 0, shellCount + 1); // Draw each shell plus the base terrain
				// I could do each of the plane qualities in one instanced call, but for some reason it is slightly slower

				mWaterShader.use();
				mWaterShader.setVector3("planePos", { chunkPos.x, mWaterHeight, chunkPos.z });
				mWaterShader.setFloat("planeWorldWidth", mChunkWidth);
				glDrawElements(GL_TRIANGLES, currPlane.getIndexCount(), GL_UNSIGNED_INT, 0); // Draw each shell plus the base terrain
			}
		}

		renderUI(displayDeltaTime);
	}

	glm::vec3 getClosestWorldPixelPos(const glm::vec3 pos, int imageIndex) {
		float stepSize{ mImageWorldSizes[imageIndex] / mImagePixelDims[imageIndex] * mArtisticParams.getTerrainScale() };
		glm::vec3 stepSizesAway = pos / stepSize;
		stepSizesAway = glm::vec3{ (int)stepSizesAway.x, (int)stepSizesAway.y, (int)stepSizesAway.z };
		return stepSizesAway * stepSize;
	}

	glm::vec3 getClosestWorldVertexPos(const glm::vec3 pos) {
		float stepSize{ mLowQualityPlane.getStepSize() * mChunkWidth };
		glm::vec3 stepSizesAway = pos / stepSize;
		stepSizesAway = glm::vec3{ (int)stepSizesAway.x, (int)stepSizesAway.y, (int)stepSizesAway.z };
		return stepSizesAway * stepSize;
	}

private:
	// The chunk collection consists of a square of chunkCount * chunkCount chunks, each having a width of chunkWidth

	// The chunks will go from high to low quality, while the far chunks will all be low quality?
	int mChunkWidth;
	int mChunkCount;

	ArtisticParamsBuffer mArtisticParams;
	TerrainParamsBuffer mTerrainParams;
	WaterParamsBuffer mWaterParams;

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
		mWaterParams.renderUI();

		ImGui::Begin("Plane Chunking");
		ImGui::DragInt("Width", &mChunkWidth, 1, 1, 100);
		ImGui::DragInt("Count", &mChunkCount, 1, 1, 100);
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
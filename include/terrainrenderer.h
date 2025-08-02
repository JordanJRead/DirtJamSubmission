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
#include "colourbuffer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "camera.h"
#include <iostream>
#include "cubemap.h"
#include "cubevertices.h"
#include <array>

constexpr int ImageCount{ 4 };
//template <int ImageCount>

class TerrainRenderer {
public:
	TerrainRenderer(int screenWidth, int screenHeight, const glm::vec3& cameraPos,
		int chunkWidth, int chunkCount, const ArtisticParamsData& artistParams, const TerrainParamsData& terrainParams, const WaterParamsData& waterParams, const ColourBufferData& colours,
		std::array<int, ImageCount> imagePixelDims, std::array<float, ImageCount> imageWorldSizes, std::array<glm::vec2, ImageCount> imageWorldPositions,
		int lowQualityPlaneVerticesPerEdge, int highQualityPlaneVerticesPerEdgeScale, float vertexQualityDropoffDistance, float shellQualityDropoffDistance, float waterHeight, float dayTime)
		: mChunkWidth{ chunkWidth }
		, mChunkCount{ chunkCount }
		
		, mArtisticParams{ artistParams }
		, mTerrainParams{ terrainParams }
		, mWaterParams{ waterParams }
		, mColours{ colours }

		, mLowQualityPlaneVerticesPerEdge{ lowQualityPlaneVerticesPerEdge }
		, mHighQualityPlaneVerticesPerEdgeScale{ highQualityPlaneVerticesPerEdgeScale }

		, mLowQualityPlane{ mLowQualityPlaneVerticesPerEdge }
		, mHighQualityPlane{ mHighQualityPlaneVerticesPerEdgeScale }

		, mTerrainImageShader{ "assets/shaders/terrainimage.vert", "assets/shaders/terrainimage.frag" }
		, mTerrainShader{ "assets/shaders/terrain.vert", "assets/shaders/terrain.frag" }
		, mWaterShader{ "assets/shaders/water.vert", "assets/shaders/water.frag" }
		, mSkyboxShader{ "assets/shaders/skybox.vert", "assets/shaders/skybox.frag" }

		, mImageWorldPositions{ imageWorldPositions }
		, mImagePixelDims{ imagePixelDims }
		, mImageWorldSizes{ imageWorldSizes }

		, mVertexQualityDropoffDistance{ vertexQualityDropoffDistance }
		, mShellQualityDropoffDistance{ shellQualityDropoffDistance }

		, mImages{ {
			{mImagePixelDims[0], mImageWorldSizes[0], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 0)},
			{mImagePixelDims[1], mImageWorldSizes[1], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 1)},
			{mImagePixelDims[2], mImageWorldSizes[2], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 2)},
			{mImagePixelDims[3], mImageWorldSizes[3], screenWidth, screenHeight, getClosestWorldPixelPos(cameraPos, 3)}
		} }

		, mWaterHeight{ waterHeight }

		, mDaySkybox{ {
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_2_Left+X.png",
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_3_Right-X.png",
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_4_Up+Y.png",
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_5_Down-Y.png",
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_0_Front+Z.png",
				"assets/AllSkyFree/Epic_GloriousPink/Epic_GloriousPink_Cam_1_Back-Z.png"
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_2_Left+X.png",
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_3_Right-X.png",
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_4_Up+Y.png",
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_5_Down-Y.png",
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_0_Front+Z.png",
				//"assets/AllSkyFree/Epic_BlueSunset/Epic_BlueSunset_Cam_1_Back-Z.png"
			} }
		, mNightSkybox{ {
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_2_Left+X.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_3_Right-X.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_4_Up+Y.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_5_Down-Y.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_0_Front+Z.png",
				"assets/AllSkyFree/Night MoonBurst/Night Moon Burst_Cam_1_Back-Z.png"
			} }
		, mDayTime{ dayTime }
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
		glm::vec3 dirToSun{ getDirToSun() };
		mWaterShader.use();
		mWaterShader.setInt("skybox", 7);
		mWaterShader.setVector3("dirToLight", dirToSun);
		mTerrainShader.use();
		mTerrainShader.setInt("skybox", 7);
		mTerrainShader.setInt("imageCount", ImageCount);
		mTerrainShader.setVector3("dirToLight", dirToSun);
		for (int i{ 0 }; i < ImageCount; ++i) {
			std::string indexString{ std::to_string(i) };

			mTerrainShader.use();
			mTerrainShader.setInt("images[" + indexString + "]", i);
			mTerrainShader.setFloat("imageScales[" + indexString + "]", mImageWorldSizes[i]);
			mTerrainShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);

			mWaterShader.use();
			mWaterShader.setInt("images[" + indexString + "]", i);
			mWaterShader.setFloat("imageScales[" + indexString + "]", mImageWorldSizes[i]);
			mWaterShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);

			mImages[i].updateTexture(mScreenQuad, mTerrainImageShader);
		}

		mSkyboxShader.use();
		mSkyboxShader.setInt("skybox", 7);
		mSkyboxShader.setVector3("dirToLight", dirToSun);
	}

	void render(const Camera& camera, double displayDeltaTime, float time) {
		bool hasTerrainChanged{ mTerrainParams.updateGPU(false) };
		mArtisticParams.updateGPU(false);
		mWaterParams.updateGPU(false);
		mColours.updateGPU(false);
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

				mTerrainShader.use();
				mTerrainShader.setFloat("imageScales[" + indexString + "]", mImageWorldSizes[i]);

				mWaterShader.use();
				mWaterShader.setFloat("imageScales[" + indexString + "]", mImageWorldSizes[i]);

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

				mTerrainShader.use();
				mTerrainShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);

				mWaterShader.use();
				mWaterShader.setVector2("imagePositions[" + indexString + "]", mImageWorldPositions[i]);

				hasImageChanged = true;
			}

			if (hasImageChanged || hasTerrainChanged) {
				mImages[i].updateTexture(mScreenQuad, mTerrainImageShader); // binds another shader
				mTerrainShader.use();
			}
		}

		glm::vec3 dirToSun{ getDirToSun() };
		// Render skybox
		mSkyboxShader.use();
		mSkyboxShader.setMatrix4("view", camera.getViewMatrix());
		mSkyboxShader.setMatrix4("proj", camera.getProjectionMatrix());
		mSkyboxShader.setVector3("dirToLight", dirToSun);
		mDaySkybox.bindTexture(7);
		mCubeVertices.useVertexArray();
		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, mCubeVertices.getIndexCount(), GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);

		mTerrainShader.use();
		mTerrainShader.setMatrix4("view", camera.getViewMatrix());
		mTerrainShader.setMatrix4("proj", camera.getProjectionMatrix());
		mTerrainShader.setVector3("cameraPos", camera.getPosition());
		mTerrainShader.setFloat("waterHeight", mWaterHeight);
		mTerrainShader.setVector3("dirToLight", dirToSun);
		mWaterShader.use();
		mWaterShader.setMatrix4("view", camera.getViewMatrix());
		mWaterShader.setMatrix4("proj", camera.getProjectionMatrix());
		mWaterShader.setVector3("cameraPos", camera.getPosition());
		mWaterShader.setFloat("time", time);
		mWaterShader.setVector3("dirToLight", dirToSun);
		for (int i{ 0 }; i < mImages.size(); ++i) {
			mImages[i].bindImage(i);
		}

		float maxChunkHeight{ getMaxHeight() };

		float minChunkHeight{ getMinHeight() };

		glm::vec3 cameraForward{ camera.getForward() };
		// For each chunk
 		for (int x{ -mChunkCount / 2 }; x <= mChunkCount / 2; ++x) {
			for (int z{ -mChunkCount / 2 }; z <= mChunkCount / 2; ++z) {

				mTerrainShader.use();
				glm::vec3 chunkPos{ getClosestWorldVertexPos(camera.getPosition()) - glm::vec3(x * mChunkWidth, 0, z * mChunkWidth) };

				// Frustum culling
				std::array<float, 2> xVals{ chunkPos.x - mChunkWidth / 2.0, chunkPos.x + mChunkWidth / 2.0 };
				std::array<float, 2> yVals{ minChunkHeight - mChunkWidth / 2.0, maxChunkHeight + mChunkWidth / 2.0 };
				std::array<float, 2> zVals{ chunkPos.z - mChunkWidth / 2.0, chunkPos.z + mChunkWidth / 2.0 };

				bool isVisible{ false };
				for (float x : xVals) {
					for (float y : yVals) {
						for (float z : zVals) {
							glm::vec3 corner{ x, y, z };
							glm::vec3 viewDir{ glm::normalize(corner - camera.getPosition()) };
							if (glm::dot(viewDir, cameraForward) > 0) {
								isVisible = true;
								break;
							}
						}
						if (isVisible)
							break;
					}
					if (isVisible)
						break;
				}

				if (isVisible) {
					float chunkDist{ glm::length(chunkPos - camera.getPosition()) };
					bool highQuality{ !(chunkDist > mVertexQualityDropoffDistance) };
					Plane& currPlane{ highQuality ? mHighQualityPlane : mLowQualityPlane };

					mTerrainShader.setVector3("planePos", { chunkPos.x, 0, chunkPos.z });
					mTerrainShader.setFloat("planeWorldWidth", mChunkWidth);

					currPlane.useVertexArray();

					// LOD shell count
					int shellCount{ mArtisticParams.getShellCount() };
					if (chunkDist > mShellQualityDropoffDistance * 4) {
						shellCount = mArtisticParams.getShellCount() > 3 ? 3 : mArtisticParams.getShellCount();
						mArtisticParams.forceShaderShellCount(shellCount);
					}
					if (chunkDist > mShellQualityDropoffDistance * 2) {
						shellCount = mArtisticParams.getShellCount() > 7 ? 7 : mArtisticParams.getShellCount();
						mArtisticParams.forceShaderShellCount(shellCount);
					}
					else if (chunkDist > mShellQualityDropoffDistance) {
						shellCount = mArtisticParams.getShellCount() > 10 ? 10 : mArtisticParams.getShellCount();
						mArtisticParams.forceShaderShellCount(shellCount);
					}

					// Draw terrain
					// glInstanceID is 1 greater than the shellIndex (base terrain is -1 shell index, first shell is 0 shell index)
					glDrawElementsInstanced(GL_TRIANGLES, currPlane.getIndexCount(), GL_UNSIGNED_INT, 0, shellCount + 1); // Draw each shell plus the base terrain
					// I could do each of the plane qualities in one instanced call, but for some reason it is slightly slower

					// Draw water
					mWaterShader.use();
					mWaterShader.setVector3("planePos", { chunkPos.x, mWaterHeight, chunkPos.z });
					mWaterShader.setFloat("planeWorldWidth", mChunkWidth);
					glDrawElements(GL_TRIANGLES, currPlane.getIndexCount(), GL_UNSIGNED_INT, 0); // Draw each shell plus the base terrain

					mArtisticParams.fixShellCount();
				}
			}
		}

		if (mIsUIVisible)
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

	void toggleUI() {
		mIsUIVisible = !mIsUIVisible;
	}

private:
	// The chunk collection consists of a square of chunkCount * chunkCount chunks, each having a width of chunkWidth

	// The chunks will go from high to low quality, while the far chunks will all be low quality?
	int mChunkWidth;
	int mChunkCount;

	ArtisticParamsBuffer mArtisticParams;
	TerrainParamsBuffer mTerrainParams;
	WaterParamsBuffer mWaterParams;
	ColourBuffer mColours;
	bool mIsUIVisible{ true };

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
	Shader mSkyboxShader;
	Cubemap mDaySkybox;
	Cubemap mNightSkybox;
	CubeVertices mCubeVertices;
	float mDayTime;
	//glm::vec3 mDirToLight{ -0.008373, 0.089878, 0.995917 };

	Plane mLowQualityPlane;
	Plane mHighQualityPlane;

	float mVertexQualityDropoffDistance;
	float mShellQualityDropoffDistance;

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
		mColours.renderUI();

		ImGui::Begin("Plane Chunking");
		ImGui::DragInt("Width", &mChunkWidth, 1, 1, 100);
		ImGui::DragInt("Count", &mChunkCount, 1, 1, 100);
		ImGui::DragInt("Low quality plane vertices", &mLowQualityPlaneVerticesPerEdge, 1, 2, 1000);
		ImGui::DragInt("High quality plane quality scale", &mHighQualityPlaneVerticesPerEdgeScale, 1, 2, 1000);
		ImGui::DragFloat("Vertex LOD dist", &mVertexQualityDropoffDistance, 1, 1, 1000);
		ImGui::DragFloat("Shell LOD dist", &mShellQualityDropoffDistance, 1, 1, 1000);
		ImGui::DragFloat("Water height", &mWaterHeight, 0.1);
		ImGui::End();

		ImGui::Begin("Day");
		ImGui::DragFloat("Day time", &mDayTime, 0.001, 0.1, 0.9);
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

	float getMaxHeight() {
		glm::vec3 mountain = { 1, 0, 0 };

		mountain.x *= mountain.x;
		mountain.x *= mountain.x;
		mountain.x = mountain.x * 0.95 + 0.05;

		glm::vec3 offset = { 1, 0, 0 };

		offset.x = offset.x < 0.5 ? (16 * offset.x * offset.x * offset.x * offset.x * offset.x) : 1 - pow(-2 * offset.x + 2, 5.0) / 2.0;

		offset *= 20;

		glm::vec3 terrainInfo = { 0, 0, 0 };

		float amplitude = mTerrainParams.getInitialAmplitude();

		for (int i{ 0 }; i < mTerrainParams.getOctaveCount(); ++i) {
			glm::vec3 perlinData = { 1, 0, 0 };

			terrainInfo.x += amplitude * perlinData.x;

			amplitude *= mTerrainParams.getAmplitudeDecay();
		}

		glm::vec3 finalOutput = { 0, 0, 0 };
		finalOutput.x = terrainInfo.x * mountain.x;

		finalOutput.x += offset.x;
		return finalOutput.x;
	}

	float getMinHeight() {
		glm::vec3 mountain = { 0, 0, 0 };

		mountain.x *= mountain.x;
		mountain.x *= mountain.x;
		mountain.x = mountain.x * 0.95 + 0.05;

		glm::vec3 offset = { 0, 0, 0 };

		offset.x = offset.x < 0.5 ? (16 * offset.x * offset.x * offset.x * offset.x * offset.x) : 1 - pow(-2 * offset.x + 2, 5.0) / 2.0;

		offset *= 20;

		glm::vec3 terrainInfo = { 0, 0, 0 };

		float amplitude = mTerrainParams.getInitialAmplitude();

		for (int i{ 0 }; i < mTerrainParams.getOctaveCount(); ++i) {
			glm::vec3 perlinData = { 0, 0, 0 };

			terrainInfo.x += amplitude * perlinData.x;

			amplitude *= mTerrainParams.getAmplitudeDecay();
		}

		glm::vec3 finalOutput = { 0, 0, 0 };
		finalOutput.x = terrainInfo.x * mountain.x;

		finalOutput.x += offset.x;
		return finalOutput.x;
	}

	glm::vec3 getDirToSun() {
		float theta{ mDayTime * glm::pi<float>() };
		return glm::vec3{ glm::cos(theta), glm::sin(theta), 0 };
	}
};

#endif
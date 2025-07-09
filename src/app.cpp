#include "app.h"
#include <vector>
#include "vertexarray.h"
#include <iostream>
#include "plane.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "terrainparamsbuffer.h"
#include "terrainnoise.h"

App::App(int screenWidth, int screenHeight, GLFWwindow* window)
	: mCamera{ screenWidth, screenHeight }
	, mTerrainShader{ "shaders/terrain.vert", "shaders/terrain.frag" }
	, mGridShader{ "shaders/grid.vert", "shaders/grid.frag" }
	, mWindow{ window }
	, mScreenWidth{ screenWidth }
	, mScreenHeight{ screenHeight }
{
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetCursorPosCallback(mWindow, mouseCallback);
	glfwSetKeyCallback(mWindow, keyCallback);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_MULTISAMPLE);
	glViewport(0, 0, screenWidth, screenHeight);
	glPointSize(5);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5f, 0.5f, 0.5f, 1);
	mTerrainShader.use();
	mTerrainShader.setInt("terrainImage", 0);
	mTerrainShader.setInt("terrainImageBad", 1);
}

void App::handleInput() {
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(mWindow, 1);
	}
}

void App::loop() {
	double deltaTime{ 0 };
	double prevFrame{ glfwGetTime() };

	bool wireModeGUI{ false };
	bool displayGridGUI{ true };
	int planeWidthGUI{ 50 };
	int planeVertexDensityGUI{ 60 };
	int labelGUI{ 0 };
	float maxFogDistGUI{ 25 };
	float colorDotCutoffGUI{ 0.5 };
	float textureScaleGUI{ 90 };
	float extrudePerShellGUI{ 0.01 };
	float cutoffLossPerShellGUI{ 0.2 };
	float shellCutoffBaseGUI{ 0.2 };
	int maxShellCountGUI{ 3 };

	TerrainParamsBuffer terrainParameters{ 15, 10.6, 0.47, 2.02 };
	Terrain terrainImageGenerator{ 1024 * 9, 2, mScreenWidth, mScreenHeight };
	Terrain terrainImageGeneratorBad{ 1024 * 6, 16, mScreenWidth, mScreenHeight };

	bool perFragNormalsGUI{ true };

	Plane terrainPlane{ planeWidthGUI, planeVertexDensityGUI, {}, 0};
	Plane worldGridPlane{ 100, 1, {}, 0 };
	terrainPlane.useVAO();

	glfwSwapInterval(0);

	double startTime{ glfwGetTime() };
	double displayDeltaTime{ deltaTime };
	double prevTimeFrac{ 0.9 };
	while (!glfwWindowShouldClose(mWindow)) {

		double currTime{ glfwGetTime() };
		double currTimeFrac{ currTime - (long)currTime };
		if (currTimeFrac < prevTimeFrac) {
			displayDeltaTime = deltaTime;
		}
		prevTimeFrac = currTimeFrac;

		deltaTime = glfwGetTime() - prevFrame;
		prevFrame = glfwGetTime();

		/// Input
		handleInput();
		mCamera.move(mWindow, (float)deltaTime);

		/// Rendering
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Terrain
		mTerrainShader.use();
		mTerrainShader.setMatrix4("view", mCamera.getViewMatrix());
		mTerrainShader.setMatrix4("proj", mCamera.getProjectionMatrix());

		// Plane variables
		if (planeWidthGUI != terrainPlane.getWidth() || planeVertexDensityGUI != terrainPlane.getVertexDensity()) {
			// Handle invalid data
			if (planeWidthGUI <= 0)
				planeWidthGUI = 1;
			if (planeVertexDensityGUI <= 0)
				planeVertexDensityGUI = 1;
			if (planeWidthGUI == 1 && planeVertexDensityGUI == 1)
				planeVertexDensityGUI = 2;
			terrainPlane.rebuild(planeWidthGUI, planeVertexDensityGUI);
		}

		// Render plane
		mTerrainShader.setFloat("planeWorldWidth", terrainPlane.getWidth());
		mTerrainShader.setFloat("samplingScale", *terrainImageGenerator.getScalePtr() * 22);
		mTerrainShader.setFloat("samplingScaleBad", *terrainImageGeneratorBad.getScalePtr() * 22);
		mTerrainShader.setFloat("maxFogDist", maxFogDistGUI);
		mTerrainShader.setFloat("extrudePerShell", extrudePerShellGUI);
		mTerrainShader.setFloat("colorDotCutoff", colorDotCutoffGUI);
		mTerrainShader.setFloat("textureScale", textureScaleGUI);
		mTerrainShader.setFloat("cutoffLossPerShell", cutoffLossPerShellGUI);
		mTerrainShader.setFloat("cutoffBase", shellCutoffBaseGUI);
		mTerrainShader.setInt("maxShellCount", maxShellCountGUI);

		terrainPlane.useVAO();
		terrainImageGenerator.bindImage(0);
		terrainImageGeneratorBad.bindImage(1);

		for (int shellI{ 0 }; shellI <= maxShellCountGUI; ++shellI) {
			mTerrainShader.setInt("shellIndex", shellI);
			glDrawElements(wireModeGUI ? GL_LINES : GL_TRIANGLES, terrainPlane.getIndexCount(), GL_UNSIGNED_INT, 0);
		}


		// Grid
		if (displayGridGUI) {
			mGridShader.use();
			mGridShader.setMatrix4("view", mCamera.getViewMatrix());
			mGridShader.setMatrix4("proj", mCamera.getProjectionMatrix());
			mGridShader.setFloat("scale", worldGridPlane.getWidth());
			worldGridPlane.useVAO();
			glDrawElements(GL_TRIANGLES, worldGridPlane.getIndexCount(), GL_UNSIGNED_INT, 0);
		}

		// ImGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("TERRAIN PARAMETERS");
		ImGui::Text("FPS: %f", 1 / displayDeltaTime);

		ImGui::DragInt("Octaves", terrainParameters.getOctaveCountPtr(), 0.1f, 1, 50);
		ImGui::DragFloat("Amplitude", terrainParameters.getInitialAmplitudePtr(), 0.1f);
		ImGui::DragFloat("Amplitude decay", terrainParameters.getAmplitudeDecayPtr(), 0.0001f);
		ImGui::DragFloat("Spread", terrainParameters.getSpreadFactorPtr(), 0.001f);
		ImGui::DragFloat("Visibility", &maxFogDistGUI, 1, 1, 100);
		ImGui::DragFloat("Color dot", &colorDotCutoffGUI, 0.01, 0, 1);
		ImGui::End();

		ImGui::Begin("SHELL TEXTURE PARAMETERS");
		ImGui::DragFloat("Texture scale", &textureScaleGUI);
		ImGui::DragFloat("Shell extrusion", &extrudePerShellGUI, 0.001, 0, 1);
		ImGui::DragFloat("Shell falloff", &cutoffLossPerShellGUI, 0.01, 0, 1);
		ImGui::DragFloat("Shell falloff base", &shellCutoffBaseGUI, 0.01, 0, 1);
		ImGui::DragInt("Shell count", &maxShellCountGUI, 0.1, 0, 10);
		ImGui::End();

		ImGui::Begin("PLANE PARAMETERS");
		ImGui::Checkbox("Wire", &wireModeGUI);
		ImGui::Checkbox("Grid", &displayGridGUI);
		ImGui::InputInt("Plane width", &planeWidthGUI, 1, 10);
		ImGui::InputInt("Plane vertex density", &planeVertexDensityGUI, 1, 10);
		ImGui::End();

		ImGui::Begin("TERRAIN IMAGE");
		ImGui::InputInt("Pixel width", terrainImageGenerator.getPixelDimPtr(), 100, 1000);
		ImGui::DragFloat("Quality - Size", terrainImageGenerator.getScalePtr(), 0.1f, 0.1f, 100);
		ImGui::InputInt("Pixel width bad image", terrainImageGeneratorBad.getPixelDimPtr(), 100, 1000);
		ImGui::DragFloat("Quality - Size bad image", terrainImageGeneratorBad.getScalePtr(), 0.1f, 0.1f, 100);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update terrain image
		bool paramsChanged{ terrainParameters.updateGPU() };
		terrainImageGenerator.updateGPU(paramsChanged);
		terrainImageGeneratorBad.updateGPU(paramsChanged);

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
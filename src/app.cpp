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
	mTerrainShader.setInt("terrainImage", 0);
}

void App::handleInput() {
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(mWindow, 1);
	}
}

void App::loop() {
	double deltaTime{ 0 };
	double prevFrame{ glfwGetTime() };

	bool wireFrameGUI{ false };
	int planeWidthGUI{ 100 };
	int planeVertexDensityGUI{ 40 };
	int labelGUI{ 0 };
	float samplingScaleGUI{ 41.4 };

	TerrainParamsBuffer terrainParameters{ 7, 10.6, 0.5, 2.02 };
	Terrain terrainImageGenerator{ 1024, 2, mScreenWidth, mScreenHeight };

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
		mTerrainShader.setFloat("samplingScale", samplingScaleGUI);

		terrainPlane.useVAO();
		terrainImageGenerator.bindImage(0);
		glDrawElements(wireFrameGUI ? GL_POINTS : GL_TRIANGLES, terrainPlane.getIndexCount(), GL_UNSIGNED_INT, 0);

		// Grid
		mGridShader.use();
		mGridShader.setMatrix4("view", mCamera.getViewMatrix());
		mGridShader.setMatrix4("proj", mCamera.getProjectionMatrix());
		mGridShader.setFloat("scale", worldGridPlane.getWidth());
		worldGridPlane.useVAO();
		glDrawElements(GL_TRIANGLES, worldGridPlane.getIndexCount(), GL_UNSIGNED_INT, 0);

		// ImGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("TERRAIN PARAMETERS");
		ImGui::Text("FPS: %f", 1 / displayDeltaTime);

		ImGui::DragInt("Octaves", terrainParameters.getOctaveCountPtr(), 0.1f, 1, 50);
		ImGui::DragFloat("Amplitude", terrainParameters.getInitialAmplitudePtr(), 0.1f);
		ImGui::DragFloat("Amplitude decay", terrainParameters.getAmplitudeDecayPtr(), 0.03f);
		ImGui::DragFloat("Spread", terrainParameters.getSpreadFactorPtr(), 0.01f);
		ImGui::DragFloat("Sampling scale", &samplingScaleGUI, 0.1f);
		ImGui::End();

		ImGui::Begin("PLANE PARAMETERS");
		ImGui::Checkbox("Wire", &wireFrameGUI);
		ImGui::InputInt("Plane width", &planeWidthGUI, 1, 10);
		ImGui::InputInt("Plane vertex density", &planeVertexDensityGUI, 1, 10);
		ImGui::End();

		ImGui::Begin("TERRAIN IMAGE");
		ImGui::InputInt("Pixel width", terrainImageGenerator.getPixelDimPtr(), 100, 1000);
		ImGui::DragFloat("Scale", terrainImageGenerator.getScalePtr());
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update terrain image
		terrainImageGenerator.updateGPU(terrainParameters.updateGPU());

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
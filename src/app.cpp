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
	int planeVertexDensityGUI{ 20 };
	int labelGUI{ 0 };

	TerrainParamsBuffer params{ 1, 3, 0.5, 2 };
	Terrain terrain{ 1024, 5, mScreenWidth, mScreenHeight };

	bool perFragNormalsGUI{ true };
	float latticeWidthGUI{ 3 };

	Plane terrainPlane{ planeWidthGUI, planeVertexDensityGUI, {}, 0};
	Plane worldGridPlane{ 100, 1, {}, 0 };
	terrainPlane.useVAO();

	glfwSwapInterval(0);
	while (!glfwWindowShouldClose(mWindow)) {
		deltaTime = glfwGetTime() - prevFrame;
		prevFrame = glfwGetTime();

		terrain.updateGPU();

		/// Input
		handleInput();
		mCamera.move(mWindow, (float)deltaTime);

		/// Rendering
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

		// Terrain variables
		params.updateGPU();

		mTerrainShader.setBool("perFragNormals", perFragNormalsGUI);

		// Render plane
		mTerrainShader.setFloat("scale", terrainPlane.getWidth());
		mTerrainShader.setFloat("latticeWidth", latticeWidthGUI);

		terrainPlane.useVAO();
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

		ImGui::Begin("TEST WINDOW");
		ImGui::Text("FPS: %f", 1 / deltaTime);
		ImGui::Checkbox("Wire", &wireFrameGUI);
		ImGui::Checkbox("Frag normals", &perFragNormalsGUI);
		ImGui::InputInt("Plane width", &planeWidthGUI, 1, 10);
		ImGui::InputInt("Plane vertex density", &planeVertexDensityGUI, 1, 10);
		ImGui::InputInt("Label", &labelGUI, 1, 10);

		ImGui::DragInt("Octaves", params.getOctaveCountPtr(), 0.1f, 1, 50);
		ImGui::DragFloat("Amplitude", params.getInitialAmplitudePtr(), 0.1f);
		ImGui::DragFloat("Amplitude decay", params.getAmplitudeDecayPtr(), 0.03f);
		ImGui::DragFloat("Spread", params.getSpreadFactorPtr(), 0.01f);
		ImGui::DragFloat("Lattice width", &latticeWidthGUI, 0.1f);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
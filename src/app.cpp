#include "app.h"
#include <vector>
#include "vertexarray.h"
#include <iostream>
#include "plane.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "terrainparamsbuffer.h"
#include "artisticparamsbuffer.h"
#include <array>
#include "glm/glm.hpp"

App::App(int screenWidth, int screenHeight, GLFWwindow* window)
	: mCamera{ screenWidth, screenHeight }
	, mWindow{ window }
	, mScreenWidth{ screenWidth }
	, mScreenHeight{ screenHeight }
	, mTerrainRenderer{ screenWidth, screenHeight, mCamera.getPosition(), 16, 1, 1, ArtisticParamsData{ 0.2, 50, 0.8, 50, 0.2, 0.2, 3 }, TerrainParamsData{ 3, 5, 0.5, 5 }, {5000, 3000, 1000}, {5, 50, 500}, std::array<glm::vec2, 3> {glm::vec2{0}, glm::vec2{0}, glm::vec2{0}}, 2, 20, 50 }
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

	TerrainParamsBuffer terrainParameters{ 15, 10.6, 0.47, 2.02 };

	Plane worldGridPlane{ 2 };

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
		mTerrainRenderer.render(mCamera);

		// Grid
		//if (displayGridGUI) {
		//	mGridShader.use();
		//	mGridShader.setMatrix4("view", mCamera.getViewMatrix());
		//	mGridShader.setMatrix4("proj", mCamera.getProjectionMatrix());
		//	mGridShader.setFloat("scale", worldGridPlane.getWidth());
		//	worldGridPlane.useVertexArray();
		//	glDrawElements(GL_TRIANGLES, worldGridPlane.getIndexCount(), GL_UNSIGNED_INT, 0);
		//}

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
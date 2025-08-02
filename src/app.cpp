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
#include "waterparamsbuffer.h"
#include "colourbuffer.h"
#include <array>
#include "glm/glm.hpp"

App::App(int screenWidth, int screenHeight, GLFWwindow* window)
	: mCamera{ screenWidth, screenHeight, {0, 20, 0} } // x = 2883548 for farlands
	, mWindow{ window }
	, mScreenWidth{ screenWidth }
	, mScreenHeight{ screenHeight }
	, mTerrainRenderer{ screenWidth, screenHeight, mCamera.getPosition(), 16, 40, ArtisticParamsData{ 22, 328, 68, 0.875, 12, 0.117, 100, 1, 0.2, 36, 0.4 }, TerrainParamsData{ 15, 80, 0.4, 2, 0.2, 4, 0.04, 0.08, 20 }, WaterParamsData{ 24, 0.07, 0.82, 1, 1.13, 2, 1.07, 200 }, ColourBufferData{ {0.43, 0.32, 0.23}, {0.4, 0.4, 0.4}, {0, 0.5, 0}, {1, 1, 1}, {0, 0.1, 0.5}, {1, 1, 1} }, {2500, 2500, 2500, 2500}, {1, 4, 12, 36}, std::array<glm::vec2, 4> {glm::vec2{0}, glm::vec2{0}, glm::vec2{0}}, 20, 6, 110, 45, 8.3, 0.5 }
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
		mTerrainRenderer.render(mCamera, displayDeltaTime, (float)glfwGetTime());

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
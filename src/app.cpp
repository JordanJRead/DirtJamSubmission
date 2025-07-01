#include "app.h"
#include <vector>
#include "vertexarray.h"
#include <iostream>
#include "plane.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

App::App(int screenWidth, int screenHeight, GLFWwindow* window)
	: mCamera{ screenWidth, screenHeight }
	, mHelloShader{ "shaders/hello.vert", "shaders/hello.frag" }
	, mWindow{ window }
{
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetCursorPosCallback(mWindow, mouseCallback);
	glfwSetKeyCallback(mWindow, keyCallback);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_MULTISAMPLE);
	glViewport(0, 0, screenWidth, screenHeight);
}

void App::handleInput() {
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(mWindow, 1);
	}
}

void App::loop() {
	std::vector<float> vertices = {
		-0.5, -0.5, 1, 0, 0,
		 0.5, -0.5, 0, 1, 0,
		 0,    0.5, 0, 0, 1
	};

	std::vector<unsigned int> indices = {
		0, 1, 2
	};

	std::vector<int> attribs = {
		2, 3
	};
	VertexArray vertexArray;
	vertexArray.create(vertices, indices, attribs);

	vertexArray.updateVertices(vertices, indices);

	Plane plane{ 2, 3, {}, 0 };
	mHelloShader.use();
	vertexArray.use();
	plane.useVAO();

	double deltaTime{ 0 };
	double prevFrame{ glfwGetTime() };

	bool wireFrame{ false };

	while (!glfwWindowShouldClose(mWindow)) {
		glfwPollEvents();
		deltaTime = glfwGetTime() - prevFrame;
		prevFrame = glfwGetTime();

		// Input
		handleInput();
		mCamera.move(mWindow, (float)deltaTime);

		// Rendering
		mHelloShader.setMatrix4("view", mCamera.getViewMatrix());
		mHelloShader.setMatrix4("proj", mCamera.getProjectionMatrix());

		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glDrawElements(wireFrame ? GL_LINES : GL_TRIANGLES, plane.getIndexCount(), GL_UNSIGNED_INT, 0);

		ImGui::Begin("TEST WINDOW");
		ImGui::Text("Hello World");
		ImGui::Checkbox("Wire", &wireFrame);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(mWindow);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
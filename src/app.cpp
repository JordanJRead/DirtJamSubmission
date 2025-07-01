#include "app.h"
#include <vector>
#include "vertexarray.h"
#include <iostream>

App::App(int screenWidth, int screenHeight, GLFWwindow* window)
	: mCamera{ screenWidth, screenHeight }
	, mHelloShader{ "shaders/hello.vert", "shaders/hello.frag" }
	, mWindow{ window }
{
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetCursorPosCallback(mWindow, mouseCallback);
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
	VertexArray vao{ vertices, indices, attribs };

	mHelloShader.use();
	vao.use();

	double deltaTime{ 0 };
	double prevFrame{ glfwGetTime() };

	while (!glfwWindowShouldClose(mWindow)) {
		deltaTime = glfwGetTime() - prevFrame;
		prevFrame = glfwGetTime();

		// Input
		handleInput();
		mCamera.move(mWindow, (float)deltaTime);

		// Rendering
		mHelloShader.setMatrix4("view", mCamera.getViewMatrix());
		mHelloShader.setMatrix4("proj", mCamera.getProjectionMatrix());

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, vao.getIndexCount(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
	glfwTerminate();
}
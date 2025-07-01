#ifndef APP_H
#define APP_H

#include "camera.h"
#include "shader.h"
#include "GLFW/glfw3.h"]

class App {
public:
	App(int screenWidth, int screenHeight, GLFWwindow* window);
	void loop();

private:
	Camera mCamera;
	Shader mHelloShader;
	GLFWwindow* mWindow;

	static void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
		static_cast<App*>(glfwGetWindowUserPointer(window))->mCamera.mouseCallback(window, xPos, yPos);
	}
};

#endif
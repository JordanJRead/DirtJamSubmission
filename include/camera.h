#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"
#include <iostream>

class Camera {
public:
	Camera(int screenWidth, int screenHeight, const glm::vec3& position = { 0, 0, 0 }, float speed = 1, float sens = 0.005f);
	void mouseCallback(GLFWwindow* window, double xPos, double yPos, bool isCursorHidden);
	void move(GLFWwindow* window, float deltaTime);
	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
	glm::vec3 getPosition() const { return mPosition; }

private:
	float mPitch{ 0 }; // In radians
	float mYaw{ 0 }; // In radians
	float mSpeed;
	float mSens;
	glm::vec3 mPosition;
	float mAspectRatio;
	float mPrevX{ 0 };
	float mPrevY{ 0 };
	bool isFirstLook{ true };

private:
	glm::vec3 getForward() const;
};

#endif
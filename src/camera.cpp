#include "camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"

Camera::Camera(int screenWidth, int screenHeight, const glm::vec3& position, float speed, float sens)
	: mPosition{ position }
	, mSpeed{ speed } 
	, mSens{ sens }
, mAspectRatio{ static_cast<float>(screenWidth) / screenHeight }
{
	mYaw = 3.1415926535897932384626433832795 / 2.0f;
}

void Camera::mouseCallback(GLFWwindow* window, double xPos, double yPos, bool isCursorHidden) {
	static float pi{ 3.1415926535897932384626433832795 };

	if (!isCursorHidden) {
		isFirstLook = true;
		return;
	}
	if (isFirstLook) {
		isFirstLook = false;
		mPrevX = xPos;
		mPrevY = yPos;
		return;
	}
	float dx = xPos - mPrevX;
	float dy = yPos - mPrevY;

	mYaw -= dx * mSens;
	mPitch -= dy * mSens;

	if (mPitch >= pi / 2.0f)
		mPitch = pi / 2.0f - 0.001f;

	if (mPitch < -pi / 2.0f)
		mPitch = -pi / 2.0f + 0.001f;

	mPrevX = xPos;
	mPrevY = yPos;
}

glm::mat4 Camera::getViewMatrix() const {
	glm::vec3 forward{ getForward() };
	return glm::lookAt(mPosition, mPosition + forward, { 0, 1, 0 });
}

glm::mat4 Camera::getProjectionMatrix() const {
	return glm::perspective(glm::radians(90.0f), mAspectRatio, 0.1f, 100.0f);
}

glm::vec3 Camera::getForward() const {
	return {
		 glm::cos(mYaw) * glm::cos(mPitch),
		 glm::sin(mPitch),
		-glm::sin(mYaw) * glm::cos(mPitch)
	};
}

void Camera::move(GLFWwindow* window, float deltaTime) {
	glm::vec3 move{ 0, 0, 0 };
	glm::vec3 forward{ getForward() };
	glm::vec3 left{ forward.x, 0, forward.z };
	left = glm::normalize(left);
	left = glm::rotate(glm::identity<glm::mat4>(), glm::radians(90.0f), glm::vec3{0, 1, 0}) * glm::vec4{left.x, left.y, left.z, 1};

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		move += forward;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		move -= forward;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		move += left;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		move -= left;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		move += glm::vec3 { 0, 1, 0 };
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		move -= glm::vec3{ 0, 1, 0 };
	}
 	if (glm::length(move) != 0)
		move = glm::normalize(move);
	mPosition += move * mSpeed * deltaTime;
}
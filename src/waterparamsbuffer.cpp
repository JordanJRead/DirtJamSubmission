#include "waterparamsbuffer.h"
#include "glad/glad.h"
#include "imgui.h"

void WaterParamsBuffer::renderUI() {
	ImGui::Begin("Water Parameters");
	ImGui::DragInt("Wave count", &mWaveCount.mGUI, 0.1, 1, 100);
	ImGui::DragFloat("Initial amplitude", &mInitialAmplitude.mGUI, 0.005, 0.01, 1);
	ImGui::DragFloat("Amplitude multiplier", &mAmplitudeMult.mGUI, 0.001, 0, 1);
	ImGui::DragFloat("Initial frequency", &mInitialFreq.mGUI, 0.01, 0, 5);
	ImGui::DragFloat("Frequency multiplier", &mFreqMult.mGUI, 0.01, 0, 1.5);
	ImGui::DragFloat("Initial speed", &mInitialSpeed.mGUI, 0.02, 0, 20);
	ImGui::DragFloat("Speed multiplier", &mSpeedMult.mGUI, 0.007, 0, 2);
	ImGui::DragFloat("Shininess", &mSpecularExponent.mGUI);
	ImGui::End();
}

bool WaterParamsBuffer::updateGPU(bool force) {
	bool hasChanged{ false };
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);

	int offset{ 0 };

	int size = sizeof(int);
	if (mWaveCount.hasDiff() || force) {
		mWaveCount.mShader = mWaveCount.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mWaveCount.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mInitialAmplitude.hasDiff() || force) {
		mInitialAmplitude.mShader = mInitialAmplitude.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mInitialAmplitude.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mAmplitudeMult.hasDiff() || force) {
		mAmplitudeMult.mShader = mAmplitudeMult.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mAmplitudeMult.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mInitialFreq.hasDiff() || force) {
		mInitialFreq.mShader = mInitialFreq.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mInitialFreq.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mFreqMult.hasDiff() || force) {
		mFreqMult.mShader = mFreqMult.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mFreqMult.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mInitialSpeed.hasDiff() || force) {
		mInitialSpeed.mShader = mInitialSpeed.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mInitialSpeed.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mSpeedMult.hasDiff() || force) {
		mSpeedMult.mShader = mSpeedMult.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mSpeedMult.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mSpecularExponent.hasDiff() || force) {
		mSpecularExponent.mShader = mSpecularExponent.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mSpecularExponent.mShader);
		hasChanged = true;
	}
	offset += size;

	return hasChanged;
}
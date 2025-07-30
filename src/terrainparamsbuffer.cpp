#include "terrainparamsbuffer.h"
#include "glad/glad.h"
#include "imgui.h"

void TerrainParamsBuffer::renderUI() {
	ImGui::Begin("Terrain Parameters");
	ImGui::DragInt("Octave count", &mOctaveCount.mGUI, 0.1, 1, 30);
	ImGui::DragFloat("Amplitude", &mInitialAmplitude.mGUI, 0.7, 0, 500);
	ImGui::DragFloat("Amplitude decay", &mAmplitudeDecay.mGUI, 0.0005, 0, 100);
	ImGui::DragFloat("Spread factor", &mSpreadFactor.mGUI, 0.001, 0, 100);
	ImGui::DragFloat("Mountain frequency", &mMountainFrequency.mGUI, 0.003, 0, 2);
	ImGui::DragFloat("Mountain exponent", &mMountainExponent.mGUI, 0.01, 0.1, 30);
	ImGui::DragFloat("Anti flat factor", &mAntiFlatFactor.mGUI, 0.001, 0, 1);
	ImGui::DragFloat("Dip frequency", &mDipScale.mGUI, 0.001, 0, 2);
	ImGui::DragFloat("Dip strength", &mDipStrength.mGUI, 1, 0, 1000);
	ImGui::End();
}

bool TerrainParamsBuffer::updateGPU(bool force) {
	bool hasChanged{ false };
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);

	int offset{ 0 };

	int size{ sizeof(int) };
	if (mOctaveCount.hasDiff() || force) {
		mOctaveCount.mShader = mOctaveCount.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mOctaveCount.mShader);
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
	if (mAmplitudeDecay.hasDiff() || force) {
		mAmplitudeDecay.mShader = mAmplitudeDecay.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mAmplitudeDecay.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mSpreadFactor.hasDiff() || force) {
		mSpreadFactor.mShader = mSpreadFactor.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mSpreadFactor.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mMountainFrequency.hasDiff() || force) {
		mMountainFrequency.mShader = mMountainFrequency.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mMountainFrequency.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mMountainExponent.hasDiff() || force) {
		mMountainExponent.mShader = mMountainExponent.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mMountainExponent.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mAntiFlatFactor.hasDiff() || force) {
		mAntiFlatFactor.mShader = mAntiFlatFactor.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mAntiFlatFactor.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mDipScale.hasDiff() || force) {
		mDipScale.mShader = mDipScale.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mDipScale.mShader);
		hasChanged = true;
	}
	offset += size;

	size = sizeof(float);
	if (mDipStrength.hasDiff() || force) {
		mDipStrength.mShader = mDipStrength.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mDipStrength.mShader);
		hasChanged = true;
	}
	offset += size;

	return hasChanged;
}
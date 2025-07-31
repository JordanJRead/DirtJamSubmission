#include "colourbuffer.h"
#include "glad/glad.h"
#include "imgui.h"

void ColourBuffer::renderUI() {
	ImGui::Begin("Colours");
	ImGui::ColorPicker3("Dirt", &(mDirtColour.mGUI.x));
	ImGui::ColorPicker3("Mountain", &(mMountainColour.mGUI.x));
	ImGui::ColorPicker3("Grass", &(mGrassColour.mGUI.x));
	ImGui::ColorPicker3("Snow", &(mSnowColour.mGUI.x));
	ImGui::ColorPicker3("Water", &(mWaterColour.mGUI.x));
	ImGui::ColorPicker3("Sun", &(mSunColour.mGUI.x));
	ImGui::End();
}

bool ColourBuffer::updateGPU(bool force) {
	bool hasChanged{ false };
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);

	int offset{ 0 };

	int size = 4 * sizeof(float);
	if (mDirtColour.hasDiff() || force) {
		mDirtColour.mShader = mDirtColour.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mDirtColour.mShader);
		hasChanged = true;
	}
	offset += size;

	size = 4 * sizeof(float);
	if (mMountainColour.hasDiff() || force) {
		mMountainColour.mShader = mMountainColour.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mMountainColour.mShader);
		hasChanged = true;
	}
	offset += size;

	size = 4 * sizeof(float);
	if (mGrassColour.hasDiff() || force) {
		mGrassColour.mShader = mGrassColour.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mGrassColour.mShader);
		hasChanged = true;
	}
	offset += size;

	size = 4 * sizeof(float);
	if (mSnowColour.hasDiff() || force) {
		mSnowColour.mShader = mSnowColour.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mSnowColour.mShader);
		hasChanged = true;
	}
	offset += size;

	size = 4 * sizeof(float);
	if (mWaterColour.hasDiff() || force) {
		mWaterColour.mShader = mWaterColour.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mWaterColour.mShader);
		hasChanged = true;
	}
	offset += size;

	size = 4 * sizeof(float);
	if (mSunColour.hasDiff() || force) {
		mSunColour.mShader = mSunColour.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mSunColour.mShader);
		hasChanged = true;
	}
	offset += size;

	return hasChanged;
}
#include "artisticparamsbuffer.h"
#include "glad/glad.h"
#include <imgui.h>

void ArtisticParamsBuffer::renderUI() {
	ImGui::Begin("Artistic Parameters");
	ImGui::DragFloat("Terrain scale", &mTerrainScale.mGUI);
	ImGui::DragFloat("View distance", &mMaxViewDistance.mGUI, 1, 0, 1000);
	ImGui::DragFloat("Fog encroachment", &mFogEncroach.mGUI, 1, 0, int(mMaxViewDistance.mGUI));
	ImGui::DragFloat("Color dot cutoff", &mColorDotCutoff.mGUI, 0.005, 0, 1);
	ImGui::DragInt("Shell count", &mShellCount.mGUI, 0.1, 0, 256);
	ImGui::DragFloat("Shell max height", &mShellMaxHeight.mGUI, 0.001, 0, 10);
	ImGui::DragFloat("Shell detail", &mShellDetail.mGUI, 1, 1, 1000);
	ImGui::DragFloat("Shell max cutoff", &mShellMaxCutoff.mGUI, 0.01, 0, 1);
	ImGui::DragFloat("Shell base cutoff", &mShellBaseCutoff.mGUI, 0.01, 0, 1);
	ImGui::DragFloat("Snow height", &mSnowHeight.mGUI);
	ImGui::DragFloat("Seafoam", &mSeafoamStrength.mGUI, 0.01, 0, 10);
	ImGui::End();
}

void ArtisticParamsBuffer::updateGPU(bool force) {
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);

	int offset{ 0 };

	int size{ sizeof(float) };
	if (mTerrainScale.hasDiff() || force) {
		mTerrainScale.mShader = mTerrainScale.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mTerrainScale.mShader);
	}
	offset += size;

	size = sizeof(float);
	if (mMaxViewDistance.hasDiff() || force) {
		mMaxViewDistance.mShader = mMaxViewDistance.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mMaxViewDistance.mShader);
	}
	offset += size;

	size = sizeof(float);
	if (mFogEncroach.hasDiff() || force) {
		mFogEncroach.mShader = mFogEncroach.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mFogEncroach.mShader);
	}
	offset += size;

	size = sizeof(float);
	if (mColorDotCutoff.hasDiff() || force) {
		mColorDotCutoff.mShader = mColorDotCutoff.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mColorDotCutoff.mShader);
	}
	offset += size;

	size = sizeof(int);
	if (mShellCount.hasDiff() || force) {
		mShellCount.mShader = mShellCount.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellCount.mShader);
	}
	offset += size;

	size = sizeof(float);
	if (mShellMaxHeight.hasDiff() || force) {
		mShellMaxHeight.mShader = mShellMaxHeight.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellMaxHeight.mShader);
	}
	offset += size;

	size = sizeof(float);
	if (mShellDetail.hasDiff() || force) {
		mShellDetail.mShader = mShellDetail.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellDetail.mShader);
	}
	offset += size;

	size = sizeof(float);
	if (mShellMaxCutoff.hasDiff() || force) {
		mShellMaxCutoff.mShader = mShellMaxCutoff.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellMaxCutoff.mShader);
	}
	offset += size;

	size = sizeof(float);
	if (mShellBaseCutoff.hasDiff() || force) {
		mShellBaseCutoff.mShader = mShellBaseCutoff.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellBaseCutoff.mShader);
	}
	offset += size;

	size = sizeof(float);
	if (mSnowHeight.hasDiff() || force) {
		mSnowHeight.mShader = mSnowHeight.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mSnowHeight.mShader);
	}
	offset += size;

	size = sizeof(float);
	if (mSeafoamStrength.hasDiff() || force) {
		mSeafoamStrength.mShader = mSeafoamStrength.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mSeafoamStrength.mShader);
	}
	offset += size;
}
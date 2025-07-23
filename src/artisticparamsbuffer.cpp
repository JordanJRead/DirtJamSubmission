#include "artisticparamsbuffer.h"
#include "glad/glad.h"
#include <imgui.h>

ArtisticParamsBuffer::ArtisticParamsBuffer(float terrainScale, float maxFogDist, float colorDotCutoff, int shellCount, float shellMaxHeight, float shellDetail, float shellMaxCutoff, float shellBaseCutoff)
	: mTerrainScale{ terrainScale }
	, mMaxFogDist{ maxFogDist }
	, mColorDotCutoff{ colorDotCutoff }
	, mShellCount{ shellCount }
	, mShellMaxHeight{ shellMaxHeight }
	, mShellDetail{ shellDetail }
	, mShellMaxCutoff{ shellMaxCutoff }
	, mShellBaseCutoff{ shellBaseCutoff }
{
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 7 * sizeof(float), nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, mBUF);

	updateGPU(true);
}

//ArtisticParamsBuffer::ArtisticParamsBuffer(const ArtisticParamsBuffer& other)
//	: mExtrudePerShell{ other.mExtrudePerShell }
//	, mMaxFogDist{ other.mMaxFogDist }
//	, mColorDotCutoff{ other.mColorDotCutoff }
//	, mShellTexelScale{ other.mShellTexelScale }
//	, mShellCutoffLossPerShell{ other.mShellCutoffLossPerShell }
//	, mShellCutoffBase{ other.mShellCutoffBase }
//	, mMaxShellCount{ other.mMaxShellCount } { }

void ArtisticParamsBuffer::renderUI() {
	ImGui::Begin("Artistic Parameters");
	ImGui::DragFloat("Terrain scale", &mTerrainScale.mGUI);
	ImGui::DragFloat("View distance", &mMaxFogDist.mGUI, 1, 1, 1000);
	ImGui::DragFloat("Color dot cutoff", &mColorDotCutoff.mGUI, 0.005, 0, 1);
	ImGui::DragInt("Shell count", &mShellCount.mGUI, 0.1, 0, 256);
	ImGui::DragFloat("Shell max height", &mShellMaxHeight.mGUI, 0.001, 0, 10);
	ImGui::DragFloat("Shell detail", &mShellDetail.mGUI, 1, 1, 1000);
	ImGui::DragFloat("Shell max cutoff", &mShellMaxCutoff.mGUI, 0.01, 0, 1);
	ImGui::DragFloat("Shell base cutoff", &mShellBaseCutoff.mGUI, 0.01, 0, 1);
	ImGui::End();
}

/*
	uniform float terrainScale;
	uniform float maxFogDist;
	uniform float colorDotCutoff;
	uniform int shellCount;
	uniform float shellMaxHeight;
	uniform float shellDetail;
	uniform float shellMaxCutoff;
	uniform float shellBaseCutoff;
*/

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
	if (mMaxFogDist.hasDiff() || force) {
		mMaxFogDist.mShader = mMaxFogDist.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mMaxFogDist.mShader);
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
}
#include "artisticparamsbuffer.h"
#include "glad/glad.h"
#include <imgui.h>

ArtisticParamsBuffer::ArtisticParamsBuffer(float terrainScale, float maxFogDist, float colorDotCutoff, int shellCount, float shellMaxHeight, float shellDetail, float shellMaxCutoff, float shellBaseCutoff)
	: mTerrainScale{ terrainScale }
	, mFogStrength{ maxFogDist }
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
	ImGui::DragFloat("Terrain scale", &mTerrainScale.mCPU);
	ImGui::DragFloat("Fog strength", &mFogStrength.mCPU, 0.0001, 0, 1);
	ImGui::DragFloat("Color dot cutoff", &mColorDotCutoff.mCPU, 0.005, 0, 1);
	ImGui::DragInt("Shell count", &mShellCount.mCPU, 0.1, 0, 256);
	ImGui::DragFloat("Shell max height", &mShellMaxHeight.mCPU, 0.001, 0, 10);
	ImGui::DragFloat("Shell detail", &mShellDetail.mCPU, 1, 1, 1000);
	ImGui::DragFloat("Shell max cutoff", &mShellMaxCutoff.mCPU, 0.01, 0, 1);
	ImGui::DragFloat("Shell base cutoff", &mShellBaseCutoff.mCPU, 0.01, 0, 1);
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
		mTerrainScale.mGPU = mTerrainScale.mCPU;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mTerrainScale.mGPU);
	}
	offset += size;

	size = sizeof(float);
	if (mFogStrength.hasDiff() || force) {
		mFogStrength.mGPU = mFogStrength.mCPU;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mFogStrength.mGPU);
	}
	offset += size;

	size = sizeof(float);
	if (mColorDotCutoff.hasDiff() || force) {
		mColorDotCutoff.mGPU = mColorDotCutoff.mCPU;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mColorDotCutoff.mGPU);
	}
	offset += size;

	size = sizeof(int);
	if (mShellCount.hasDiff() || force) {
		mShellCount.mGPU = mShellCount.mCPU;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellCount.mGPU);
	}
	offset += size;

	size = sizeof(float);
	if (mShellMaxHeight.hasDiff() || force) {
		mShellMaxHeight.mGPU = mShellMaxHeight.mCPU;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellMaxHeight.mGPU);
	}
	offset += size;

	size = sizeof(float);
	if (mShellDetail.hasDiff() || force) {
		mShellDetail.mGPU = mShellDetail.mCPU;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellDetail.mGPU);
	}
	offset += size;

	size = sizeof(float);
	if (mShellMaxCutoff.hasDiff() || force) {
		mShellMaxCutoff.mGPU = mShellMaxCutoff.mCPU;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellMaxCutoff.mGPU);
	}
	offset += size;

	size = sizeof(float);
	if (mShellBaseCutoff.hasDiff() || force) {
		mShellBaseCutoff.mGPU = mShellBaseCutoff.mCPU;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellBaseCutoff.mGPU);
	}
	offset += size;
}
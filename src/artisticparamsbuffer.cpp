#include "artisticparamsbuffer.h"
#include "glad/glad.h"
#include <imgui.h>

ArtisticParamsBuffer::ArtisticParamsBuffer(float extrudePerShell, float maxFogDist, float colorDotCutoff, float shellTexelScale, float shellCutoffLossPerShell, float shellCutoffBase, int maxShellCount)
	: mExtrudePerShell{ extrudePerShell }
	, mMaxFogDist{ maxFogDist }
	, mColorDotCutoff{ colorDotCutoff }
	, mShellTexelScale{ shellTexelScale }
	, mShellCutoffLossPerShell{ shellCutoffLossPerShell }
	, mShellCutoffBase{ shellCutoffBase }
	, mMaxShellCount{ maxShellCount }
{
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 6 * sizeof(float), nullptr, GL_STATIC_DRAW);
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
	ImGui::DragFloat("Extrusion per shell", &mExtrudePerShell.mGUI, 1, 0, 10);
	ImGui::DragFloat("View distance", &mMaxFogDist.mGUI, 1, 1, 1000);
	ImGui::DragFloat("Color dot cutoff", &mColorDotCutoff.mGUI, 0.01, 0, 1);
	ImGui::DragFloat("Shell texel scale", &mShellTexelScale.mGUI, 1, 1, 1000);
	ImGui::DragFloat("Shell cutoff loss rate", &mShellCutoffLossPerShell.mGUI, 0.01, 0, 1);
	ImGui::DragFloat("Shell cutoff base", &mShellCutoffBase.mGUI, 0.01, 0, 1);
	ImGui::DragInt("Shell count (counting base)", &mMaxShellCount.mGUI, 0.1, 1, 10);
	ImGui::End();
}

void ArtisticParamsBuffer::updateGPU(bool force) {
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);

	int offset{ 0 };

	int size{ sizeof(float) };
	if (mExtrudePerShell.hasDiff() || force) {
		mExtrudePerShell.mShader = mExtrudePerShell.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mExtrudePerShell.mShader);
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

	size = sizeof(float);
	if (mShellTexelScale.hasDiff() || force) {
		mShellTexelScale.mShader = mShellTexelScale.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellTexelScale.mShader);
	}
	offset += size;

	size = sizeof(float);
	if (mShellCutoffLossPerShell.hasDiff() || force) {
		mShellCutoffLossPerShell.mShader = mShellCutoffLossPerShell.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellCutoffLossPerShell.mShader);
	}
	offset += size;

	size = sizeof(float);
	if (mShellCutoffBase.hasDiff() || force) {
		mShellCutoffBase.mShader = mShellCutoffBase.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellCutoffBase.mShader);
	}
	offset += size;

	size = sizeof(int);
	if (mMaxShellCount.hasDiff() || force) {
		mMaxShellCount.mShader = mMaxShellCount.mGUI;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mMaxShellCount.mShader);
	}
	offset += size;
}
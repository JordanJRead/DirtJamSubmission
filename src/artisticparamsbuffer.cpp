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
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, mBUF);

	updateGPU(true);
}

ArtisticParamsBuffer::ArtisticParamsBuffer(const ArtisticParamsBuffer& other)
	: mExtrudePerShell{ other.mExtrudePerShell }
	, mMaxFogDist{ other.mMaxFogDist }
	, mColorDotCutoff{ other.mColorDotCutoff }
	, mShellTexelScale{ other.mShellTexelScale }
	, mShellCutoffLossPerShell{ other.mShellCutoffLossPerShell }
	, mShellCutoffBase{ other.mShellCutoffBase }
	, mMaxShellCount{ other.mMaxShellCount } { }

void ArtisticParamsBuffer::renderUI() {
	ImGui::Begin("Artistic Parameters");
	ImGui::DragFloat("Extrusion per shell", &mExtrudePerShell.mGUI, 1, 0, 10);
	ImGui::DragFloat("View distance", &mMaxFogDist.mGUI, 1, 1, 1000);
	ImGui::DragFloat("Color dot cutoff", &mColorDotCutoff.mGUI, 0.01, 0, 1);
	ImGui::DragFloat("Shell texel scale", &mShellTexelScale.mGUI, 1, 1, 1000);
	ImGui::DragFloat("Shell cutoff loss rate", &mShellCutoffLossPerShell.mGUI, 0.01, 0, 1);
	ImGui::DragFloat("Shell cutoff base", &mShellCutoffBase.mGUI, 0.01, 0, 1);
	ImGui::DragInt("Shell count (counting base)", &mMaxShellCount.mGUI, 0.1, 0, 10);
	ImGui::End();
}

void ArtisticParamsBuffer::updateGPU(bool force) {
	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);

	int offset{ 0 };

	if (mExtrudePerShell.hasDiff() || force) {
		mExtrudePerShell.mShader = mExtrudePerShell.mGUI;
		int size{ sizeof(float) };
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mExtrudePerShell.mShader);
		offset += size;
	}

	if (mMaxFogDist.hasDiff() || force) {
		mMaxFogDist.mShader = mMaxFogDist.mGUI;
		int size{ sizeof(float) };
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mMaxFogDist.mShader);
		offset += size;
	}

	if (mColorDotCutoff.hasDiff() || force) {
		mColorDotCutoff.mShader = mColorDotCutoff.mGUI;
		int size{ sizeof(float) };
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mColorDotCutoff.mShader);
		offset += size;
	}

	if (mShellTexelScale.hasDiff() || force) {
		mShellTexelScale.mShader = mShellTexelScale.mGUI;
		int size{ sizeof(float) };
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellTexelScale.mShader);
		offset += size;
	}

	if (mShellCutoffLossPerShell.hasDiff() || force) {
		mShellCutoffLossPerShell.mShader = mShellCutoffLossPerShell.mGUI;
		int size{ sizeof(float) };
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellCutoffLossPerShell.mShader);
		offset += size;
	}

	if (mShellCutoffBase.hasDiff() || force) {
		mShellCutoffBase.mShader = mShellCutoffBase.mGUI;
		int size{ sizeof(float) };
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mShellCutoffBase.mShader);
		offset += size;
	}

	if (mMaxShellCount.hasDiff() || force) {
		mMaxShellCount.mShader = mMaxShellCount.mGUI;
		int size{ sizeof(int) };
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &mMaxShellCount.mShader);
		offset += size;
	}
}
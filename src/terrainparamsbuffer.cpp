#include "terrainparamsbuffer.h"
#include "glad/glad.h"
#include "imgui.h"

//TerrainParamsBuffer::TerrainParamsBuffer(int octaveCount, float initialAmplitude, float amplitudeDecay, float spreadFactor)
//	: mOctaveCount{ octaveCount }
//	, mInitialAmplitude{ initialAmplitude }
//	, mAmplitudeDecay{ amplitudeDecay }
//	, mSpreadFactor{ spreadFactor }
//{
//	glBindBuffer(GL_UNIFORM_BUFFER, mBUF);
//	glBufferData(GL_UNIFORM_BUFFER, sizeof(int) + 3 * sizeof(float), nullptr, GL_STATIC_DRAW);
//	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mBUF);
//	updateGPU(true);
//}

void TerrainParamsBuffer::renderUI() {
	ImGui::Begin("Terrain Parameters");
	ImGui::DragInt("Octave count", &mOctaveCount.mGUI, 0.1, 1, 30);
	ImGui::DragFloat("Amplitude", &mInitialAmplitude.mGUI, 0.7, 0, 500);
	ImGui::DragFloat("Amplitude decay", &mAmplitudeDecay.mGUI, 0.0005, 0, 100);
	ImGui::DragFloat("Spread factor", &mSpreadFactor.mGUI, 0.001, 0, 100);
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
	return hasChanged;
}
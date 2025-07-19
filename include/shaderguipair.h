#ifndef SHADER_GUI_PAIR_H
#define SHADER_GUI_PAIR_H

template <typename T>
struct ShaderGUIPair {
	ShaderGUIPair(T shader, T GUI) : mShader{ shader }, mGUI{ GUI } {}
	ShaderGUIPair(T value) : mShader{ value }, mGUI{ value } {}
	T mShader;
	T mGUI;

	bool hasDiff() { return mShader == mGUI; }
};

#endif
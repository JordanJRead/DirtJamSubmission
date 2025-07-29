#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <array>
#include <string_view>
#include "OpenGLObjects/TEX.h"
#include "glad/glad.h"

class Cubemap {
public:
	Cubemap(const std::array<std::string_view, 6>& filePaths);
	void bindTexture(int unit) {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mTex);
	}

private:
	TEX mTex;
};

#endif
#include "cubemap.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stbimage/stb_image.h"
#include <iostream>

Cubemap::Cubemap(const std::array<std::string_view, 6>& filePaths) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (int i{ 0 }; i < filePaths.size(); ++i) {
		int w;
		int h;
		int c;
		unsigned char* data{ stbi_load(filePaths[i].data(), &w, &h, &c, 3) };
		if (!data) {
			std::cerr << "ERROR OPENING CUBEMAP IMAGE " << filePaths[i] << "\n";
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, c == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
}
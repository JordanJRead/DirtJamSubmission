#include "intssbo.h"
#include "glad/glad.h"

IntSSBO::IntSSBO(int bindingIndex) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBuf);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, mBuf);
}

void IntSSBO::UploadData(const std::vector<int> data, int usage) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBuf);
	if (data.size() == 0)
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * data.size(), nullptr, usage);
	else
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * data.size(), &data[0], usage);
}
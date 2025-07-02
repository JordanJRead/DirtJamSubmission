#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <string_view>
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {
public:
	Shader(std::string_view vertPath, std::string_view fragPath);
	~Shader() { glDeleteProgram(mID); }
	void use() const { glUseProgram(mID); }

	void setMatrix4(std::string_view name, const glm::mat4& mat4) {
		glUniformMatrix4fv(glGetUniformLocation(mID, name.data()), 1, false, glm::value_ptr(mat4));
	}
	void setFloat(std::string_view name, float f) {
		glUniform1f(glGetUniformLocation(mID, name.data()), f);
	}
	void setUInt(std::string_view name, unsigned int n) {
		glUniform1ui(glGetUniformLocation(mID, name.data()), n);
	}

private:
	unsigned int mID;
};

#endif
#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>

/// <summary>
/// Wrapper class for automatically managing OpenGL object memory
/// </summary>
class VAO {
public:
	VAO() { glGenVertexArrays(1, &mID); }
	~VAO() { glDeleteVertexArrays(1, &mID); }
	operator unsigned int() const { return mID; }
	void use() const { glBindVertexArray(mID); }

	VAO(const VAO&) = delete;
	VAO(VAO&&) = delete;
	VAO& operator=(const VAO&) = delete;
	VAO& operator=(VAO&&) = delete;

private:
	unsigned int mID;
};

#endif
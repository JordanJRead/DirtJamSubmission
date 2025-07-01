#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <vector>
#include <glad/glad.h>
#include <OpenGLObjects/BUF.h>
#include <OpenGLObjects/VAO.h>

class VertexArray {
public:
	VertexArray(const std::vector<float>& vertexData, const std::vector<unsigned int>& vertexIndices, const std::vector<int>& vertexLayout);
	void use() const { mVAO.use(); }
	size_t getIndexCount() { return mIndexCount; }

private:
	VAO mVAO;
	BUF mVBO;
	BUF mEBO;
	size_t mIndexCount;
};

#endif
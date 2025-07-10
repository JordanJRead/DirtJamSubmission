#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <vector>
#include <glad/glad.h>
#include <OpenGLObjects/BUF.h>
#include <OpenGLObjects/VAO.h>

class VertexArray {
public:
	void create(const std::vector<float>& vertexData, const std::vector<unsigned int>& vertexIndices, const std::vector<int>& vertexLayout);
	void updateVertices(const std::vector<float>& vertexData, const std::vector<unsigned int>& vertexIndices);
	void use() const { mVAO.use(); }
	size_t getIndexCount() const { return mIndexCount; }

private:
	VAO mVAO;
	BUF mVBO;
	BUF mEBO;
	size_t mIndexCount;
};

#endif
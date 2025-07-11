#ifndef PLANE_H
#define PLANE_H

#include "vertexarray.h"
#include "glm/glm.hpp"
#include "glad/glad.h"

class Plane {
public:
	Plane(int verticesPerEdge);
	void rebuild(int vertexLengthCount);
	void useVertexArray() { mVertexArray.use(); };
	int getIndexCount() { return mIndexCount; }
	int getVerticesPerEdge() { return mVerticesPerEdge; }

private:
	VertexArray mVertexArray;
	int mVerticesPerEdge;
	int mIndexCount;
};

#endif
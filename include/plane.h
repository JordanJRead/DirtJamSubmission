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
	int getIndexCount() const { return mIndexCount; }
	int getVerticesPerEdge() const { return mVerticesPerEdge; }
	float getStepSize() const { return 1.0 / (mVerticesPerEdge - 1); }

private:
	VertexArray mVertexArray;
	int mVerticesPerEdge;
	int mIndexCount;
};

#endif
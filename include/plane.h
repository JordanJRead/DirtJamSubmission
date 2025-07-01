#ifndef PLANE_H
#define PLANE_H

#include "vertexarray.h"
#include "glm/glm.hpp"
#include "glad/glad.h"

class Plane {
public:
	Plane(int width, int vertexDensity, const glm::vec3& cameraPosition, float latticeSize);
	void Rebuild(float width, int vertexDensity);
	void updatePosition(const glm::vec3& cameraPosition, float latticeSize);
	void useVAO() { mVertexArray.use(); };
	int getIndexCount() { return mIndexCount; }

private:
	VertexArray mVertexArray;
	int mWidth;
	glm::vec3 mPosition;
	int mIndexCount;
};

#endif
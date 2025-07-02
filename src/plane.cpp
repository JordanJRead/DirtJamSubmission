#include "vertexarray.h"
#include "glm/glm.hpp"
#include "plane.h"
#include "random.h"

Plane::Plane(int width, int vertexDensity, const glm::vec3& cameraPosition, float latticeSize)
	: mWidth{ width }
	, mVertexDensity{ vertexDensity }
{
	updatePosition(cameraPosition, latticeSize);
	int vertexCount{ width * vertexDensity * width * vertexDensity };

	std::vector<int> attribs { 2 };
	int floatsPerVertex{ 2 };

	std::vector<float> vertices(vertexCount * floatsPerVertex);

	int verticesOnOneSide{ width * vertexDensity };
	float stepSize{ 1 / (static_cast<float>(verticesOnOneSide) - 1) };
	float currentXPos{ -0.5 };
	float currentZPos{ -0.5 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		vertices[vertexI * floatsPerVertex + 0] = currentXPos;
		vertices[vertexI * floatsPerVertex + 1] = currentZPos;

		if ((vertexI + 1) % verticesOnOneSide == 0) {
			currentXPos = -0.5;
			currentZPos += stepSize;
		}
		else {
			currentXPos += stepSize;
		}
	}

	int numOfQuads{ (verticesOnOneSide - 1) * (verticesOnOneSide - 1) };
	int numOfTriangles{ numOfQuads * 2 };
	mIndexCount = numOfTriangles * 3;
	std::vector<unsigned int> indices(mIndexCount, 0);

	int quadsFound{ 0 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		// Ignore vertices on right or top edge
		if ((vertexI + 1) % verticesOnOneSide == 0 || vertexI >= vertexCount - verticesOnOneSide)
			continue;

		indices[quadsFound * 6 + 0] = vertexI;
		indices[quadsFound * 6 + 1] = vertexI + 1;
		indices[quadsFound * 6 + 2] = vertexI + verticesOnOneSide;

		indices[quadsFound * 6 + 3] = vertexI + 1;
		indices[quadsFound * 6 + 4] = vertexI + verticesOnOneSide;
		indices[quadsFound * 6 + 5] = vertexI + 1 + verticesOnOneSide;
		++quadsFound;
	}

	mVertexArray.create(vertices, indices, attribs);
}

void Plane::rebuild(int width, int vertexDensity) {
	mWidth = width;
	mVertexDensity = vertexDensity;
	int vertexCount{ width * vertexDensity * width * vertexDensity };
	int floatsPerVertex{ 2 };

	std::vector<float> vertices(vertexCount * floatsPerVertex);

	int verticesOnOneSide{ width * vertexDensity };
	float stepSize{ 1 / (static_cast<float>(verticesOnOneSide) - 1) };
	float currentXPos{ -0.5 };
	float currentZPos{ -0.5 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		vertices[vertexI * floatsPerVertex + 0] = currentXPos;
		vertices[vertexI * floatsPerVertex + 1] = currentZPos;

		if ((vertexI + 1) % verticesOnOneSide == 0) {
			currentXPos = -0.5;
			currentZPos += stepSize;
		}
		else {
			currentXPos += stepSize;
		}
	}

	int numOfQuads{ (verticesOnOneSide - 1) * (verticesOnOneSide - 1) };
	int numOfTriangles{ numOfQuads * 2 };
	mIndexCount = numOfTriangles * 3;
	std::vector<unsigned int> indices(mIndexCount, 0);

	int quadsFound{ 0 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		// Ignore vertices on right or top edge
		if ((vertexI + 1) % verticesOnOneSide == 0 || vertexI >= vertexCount - verticesOnOneSide)
			continue;

		indices[quadsFound * 6 + 0] = vertexI;
		indices[quadsFound * 6 + 1] = vertexI + 1;
		indices[quadsFound * 6 + 2] = vertexI + verticesOnOneSide;

		indices[quadsFound * 6 + 3] = vertexI + 1;
		indices[quadsFound * 6 + 4] = vertexI + verticesOnOneSide;
		indices[quadsFound * 6 + 5] = vertexI + 1 + verticesOnOneSide;
		++quadsFound;
	}

	mVertexArray.updateVertices(vertices, indices);
}
void Plane::updatePosition(const glm::vec3& cameraPosition, float latticeSize) {

}
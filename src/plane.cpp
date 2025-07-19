#include "vertexarray.h"
#include "glm/glm.hpp"
#include "plane.h"
#include "random.h"

Plane::Plane(int verticesPerEdge)
	: mVerticesPerEdge{ verticesPerEdge }
{
	int vertexCount{ mVerticesPerEdge * mVerticesPerEdge };

	std::vector<int> attribs { 2 };
	int floatsPerVertex{ 2 };

	std::vector<float> vertices(vertexCount * floatsPerVertex);

	double stepSize{ 1.0 / mVerticesPerEdge - 1 };
	double currentXPos{ -0.5 };
	double currentZPos{ -0.5 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		vertices[vertexI * floatsPerVertex + 0] = currentXPos;
		vertices[vertexI * floatsPerVertex + 1] = currentZPos;

		if ((vertexI + 1) % mVerticesPerEdge == 0) {
			currentXPos = -0.5;
			currentZPos += stepSize;
		}
		else {
			currentXPos += stepSize;
		}
	}

	int numOfQuads{ (mVerticesPerEdge - 1) * (mVerticesPerEdge - 1) };
	int numOfTriangles{ numOfQuads * 2 };
	mIndexCount = numOfTriangles * 3;
	std::vector<unsigned int> indices(mIndexCount, 0);

	int quadsFound{ 0 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		// Ignore vertices on right or top edge
		if ((vertexI + 1) % mVerticesPerEdge == 0 || vertexI >= vertexCount - mVerticesPerEdge)
			continue;

		indices[quadsFound * 6 + 0] = vertexI;
		indices[quadsFound * 6 + 1] = vertexI + 1;
		indices[quadsFound * 6 + 2] = vertexI + mVerticesPerEdge;

		indices[quadsFound * 6 + 3] = vertexI + 1;
		indices[quadsFound * 6 + 4] = vertexI + mVerticesPerEdge;
		indices[quadsFound * 6 + 5] = vertexI + 1 + mVerticesPerEdge;
		++quadsFound;
	}

	mVertexArray.create(vertices, indices, attribs);
}

void Plane::rebuild(int verticesPerEdge) {
	mVerticesPerEdge = verticesPerEdge;
	int vertexCount{ mVerticesPerEdge * mVerticesPerEdge };
	int floatsPerVertex{ 2 };

	std::vector<float> vertices(vertexCount * floatsPerVertex);

	double stepSize{ 1.0 / (mVerticesPerEdge - 1) };
	double currentXPos{ -0.5 };
	double currentZPos{ -0.5 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		vertices[vertexI * floatsPerVertex + 0] = currentXPos;
		vertices[vertexI * floatsPerVertex + 1] = currentZPos;

		if ((vertexI + 1) % mVerticesPerEdge == 0) {
			currentXPos = -0.5;
			currentZPos += stepSize;
		}
		else {
			currentXPos += stepSize;
		}
	}

	int numOfQuads{ (mVerticesPerEdge - 1) * (mVerticesPerEdge - 1) };
	int numOfTriangles{ numOfQuads * 2 };
	mIndexCount = numOfTriangles * 3;
	std::vector<unsigned int> indices(mIndexCount, 0);

	int quadsFound{ 0 };
	for (int vertexI{ 0 }; vertexI < vertexCount; ++vertexI) {
		// Ignore vertices on right or top edge
		if ((vertexI + 1) % mVerticesPerEdge == 0 || vertexI >= vertexCount - mVerticesPerEdge)
			continue;

		indices[quadsFound * 6 + 0] = vertexI;
		indices[quadsFound * 6 + 1] = vertexI + 1;
		indices[quadsFound * 6 + 2] = vertexI + mVerticesPerEdge;

		indices[quadsFound * 6 + 3] = vertexI + 1;
		indices[quadsFound * 6 + 4] = vertexI + mVerticesPerEdge;
		indices[quadsFound * 6 + 5] = vertexI + 1 + mVerticesPerEdge;
		++quadsFound;
	}

	mVertexArray.updateVertices(vertices, indices);
}
#ifndef CUBE_VERTICES_H
#define CUBE_VERTICES_H

#include "vertexarray.h"
#include <vector>

class CubeVertices {
public:
	/*
		+Y
		|
		|
		|------+X
	   /
	  /
	 +Z
	
		   6--------------7
		  /				 /|
		 /			    / |
		2--------------3  |
		|			   |  |
		|  4___________|__5
		| / 		   | /
		|/			   |/
		0--------------1
	*/
	CubeVertices() {
		std::vector<float> vertexData;
		vertexData.reserve(8);

		for (float z : {-1, 1}) {
			for (float y : {-1, 1}) {
				for (float x : {-1, 1}) {
					vertexData.push_back(x);
					vertexData.push_back(y);
					vertexData.push_back(z);
				}
			}
		}

		std::vector<unsigned int> indices{ {
			0, 1, 2,
			1, 2, 3,
			4, 5, 6,
			5, 6, 7,
			0, 1, 4,
			1, 4, 5,
			2, 3, 6,
			3, 6, 7,
			0, 4, 2,
			4, 2, 6,
			1, 5, 3,
			5, 3, 7
		} };

		std::vector<int> layout{ {
			3
		} };

		mVertexArray.create(vertexData, indices, layout);
	}

	void useVertexArray() {
		mVertexArray.use();
	}

	unsigned int getIndexCount() {
		return mVertexArray.getIndexCount();
	}

private:
	VertexArray mVertexArray;
};

#endif
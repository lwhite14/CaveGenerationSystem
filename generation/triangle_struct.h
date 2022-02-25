#ifndef TRIANGLE_STRUCT
#define TRIANGLE_STRUCT

#include <vector>

struct Triangle
{
	int vertexIndexA;
	int vertexIndexB;
	int vertexIndexC;
	std::vector<int> vertices;

	Triangle(int a, int b, int c)
	{
		Triangle::vertexIndexA = a;
		Triangle::vertexIndexB = b;
		Triangle::vertexIndexC = c;

		vertices = std::vector<int>(3);
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
	}

	bool Contains(int vertexIndex)
	{
		return vertexIndex == vertexIndexA || vertexIndex == vertexIndexB || vertexIndex == vertexIndexC;
	}
};

#endif
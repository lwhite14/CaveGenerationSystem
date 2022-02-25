#ifndef MESH_CLASS
#define MESH_CLASS

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <unordered_set>

#include "triangle_struct.h"

class Node 
{
public:
	glm::vec3 position;
	int vertexIndex;

	Node() 
	{
		Node::position = glm::vec3(0.0f, 0.0f, 0.0f);
		Node::vertexIndex = -1;
	}

	Node(glm::vec3 position) 
	{
		Node::position = position;
		Node::vertexIndex = -1;
	}
}; // Each square will have 8 nodes, 4 control nodes and 4 normal nodes. These nodes are used for calculating the squares configuration and drawing the appropriate shape.

class ControlNode: public Node
{
public:
	bool active;
	Node above, right;

	ControlNode() 
	{
		ControlNode::position = glm::vec3(0.0f, 0.0f, 0.0f);
		ControlNode::active = false;
		ControlNode::above = Node(position + glm::vec3(0.0f, 1.0f, 0.0f) * 1.0f / 2.0f);
		ControlNode::right = Node(position + glm::vec3(1.0f, 0.0f, 0.0f) * 1.0f / 2.0f);
		ControlNode::vertexIndex = -1;
	}

	ControlNode(glm::vec3 position, bool active, float squareSize) 
	{
		ControlNode::position = position;
		ControlNode::active = active;
		ControlNode::above = Node(position + glm::vec3(0.0f, 0.0f, 1.0f) * squareSize / 2.0f);
		ControlNode::right = Node(position + glm::vec3(1.0f, 0.0f, 0.0f) * squareSize / 2.0f);
		ControlNode::vertexIndex = -1;
	}
}; // The control nodes are used for calculating the configuration of the parent square, normal nodes are exclusively used for drawing.

class Square 
{
public:
	ControlNode topLeft, topRight, bottomRight, bottomLeft;
	Node centerTop, centerRight, centerBottom, centerLeft;
	int configuration;

	Square() 
	{
		Square::topLeft = ControlNode();
		Square::topRight = ControlNode();
		Square::bottomRight = ControlNode();
		Square::bottomLeft = ControlNode();

		centerTop = Node();
		centerRight = Node();
		centerBottom = Node();
		centerLeft = Node();

		CalculateConfiguration();
	}

	Square(ControlNode topLeft, ControlNode topRight, ControlNode bottomRight, ControlNode bottomLeft)
	{
		Square::topLeft = topLeft;
		Square::topRight = topRight;
		Square::bottomRight = bottomRight;
		Square::bottomLeft = bottomLeft;

		centerTop = topLeft.right;
		centerRight = bottomRight.above;
		centerBottom = bottomLeft.right;
		centerLeft = bottomLeft.above;

		CalculateConfiguration();
	}

	void CalculateConfiguration() 
	{
		configuration = 0;
		if (topLeft.active)
			configuration += 8;
		if (topRight.active)
			configuration += 4;
		if (bottomRight.active)
			configuration += 2;
		if (bottomLeft.active)
			configuration += 1;
	}
}; // Square class, for each spot in the map there is a square object, and each one is compared with its neighbours to get the correct configuration.

class SquareGrid 
{
public:
	std::vector<std::vector<Square>> squares;

	SquareGrid() 
	{
		squares = std::vector<std::vector<Square>>();
	}

	SquareGrid(std::vector<std::vector<int>> map, float squareSize)
	{
		int nodeCountX = map.size();
		if (nodeCountX != 0) 
		{
			int nodeCountY = map[map.size() - 1].size();
			float mapWidth = nodeCountX * squareSize;
			float mapHeight = nodeCountY * squareSize;

			std::vector<std::vector<ControlNode>> controlNodes = std::vector<std::vector<ControlNode>>(nodeCountX, std::vector<ControlNode>(nodeCountY));;

			for (int x = 0; x < nodeCountX; x++) 
			{
				for (int y = 0; y < nodeCountY; y++)
				{
					glm::vec3 pos = glm::vec3(mapWidth/2 + x * squareSize + squareSize/2, 0.0f, -mapHeight/2 + y * squareSize + squareSize/2);
					controlNodes[x][y] = ControlNode(pos, map[x][y] == 1, squareSize);
				}
			}

			squares = std::vector<std::vector<Square>>(nodeCountX - 1, std::vector<Square>(nodeCountY - 1));
			for (int x = 0; x < nodeCountX - 1; x++)
			{
				for (int y = 0; y < nodeCountY - 1; y++)
				{
					squares[x][y] = Square(controlNodes[x][y + 1], controlNodes[x + 1][y + 1], controlNodes[x + 1][y], controlNodes[x][y]);
				}
			}
		}
	}
}; // This class creates and holds all the squares (these squares hold the control and normal nodes) given a map vector and squaresize. 

class MeshGenerator 
{
public:
	// Need all the squares the cellular automata algorithm creates.
	SquareGrid squareGrid;

	// Floor positions and accompanying vertex indexes.
	std::vector<glm::vec3> vertices;
	std::vector<int> triangles;

	// Wall positions and accompanying vertex indexes.
	std::vector<glm::vec3> wallVertices;
	std::vector<int> wallTriangles;

	std::map<int, std::vector<Triangle>> triangleDictionary;
	std::vector<std::vector<int>> outlines;	
	std::unordered_set<int> checkedVertices; // Store which vertices have already been checked.

	MeshGenerator(std::vector<std::vector<int>> map, float squareSize) 
	{
		triangleDictionary = std::map<int, std::vector<Triangle>>();
		outlines = std::vector<std::vector<int>>();
		checkedVertices = std::unordered_set<int>();
		GenerateMesh(map, squareSize);
	}

	void CreateFinalVerticesLists(std::vector<std::vector<GLfloat>>& finalVerticesFloor, std::vector<std::vector<GLfloat>>& finalVerticesWalls)
	{
		for (unsigned int i = 0; i < triangles.size() / 3; i++)
		{
			finalVerticesFloor.push_back(std::vector<GLfloat>());
		}
		for (unsigned int i = 0; i < finalVerticesFloor.size(); i++)
		{
			finalVerticesFloor[i].push_back(vertices[triangles[(i * 3)]].x);
			finalVerticesFloor[i].push_back(vertices[triangles[(i * 3)]].y);
			finalVerticesFloor[i].push_back(vertices[triangles[(i * 3)]].z);
			finalVerticesFloor[i].push_back(0.22f);
			finalVerticesFloor[i].push_back(0.22f);
			finalVerticesFloor[i].push_back(0.22f);

			finalVerticesFloor[i].push_back(vertices[triangles[(i * 3) + 1]].x);
			finalVerticesFloor[i].push_back(vertices[triangles[(i * 3) + 1]].y);
			finalVerticesFloor[i].push_back(vertices[triangles[(i * 3) + 1]].z);
			finalVerticesFloor[i].push_back(0.22f);
			finalVerticesFloor[i].push_back(0.22f);
			finalVerticesFloor[i].push_back(0.22f);

			finalVerticesFloor[i].push_back(vertices[triangles[(i * 3) + 2]].x);
			finalVerticesFloor[i].push_back(vertices[triangles[(i * 3) + 2]].y);
			finalVerticesFloor[i].push_back(vertices[triangles[(i * 3) + 2]].z);
			finalVerticesFloor[i].push_back(0.22f);
			finalVerticesFloor[i].push_back(0.22f);
			finalVerticesFloor[i].push_back(0.22f);
		}

		for (unsigned int i = 0; i < wallTriangles.size() / 3; i++)
		{
			finalVerticesWalls.push_back(std::vector<GLfloat>());
		}
		for (unsigned int i = 0; i < finalVerticesWalls.size(); i++)
		{
			finalVerticesWalls[i].push_back(wallVertices[wallTriangles[(i * 3)]].x);
			finalVerticesWalls[i].push_back(wallVertices[wallTriangles[(i * 3)]].y);
			finalVerticesWalls[i].push_back(wallVertices[wallTriangles[(i * 3)]].z);
			finalVerticesWalls[i].push_back(1.0f);
			finalVerticesWalls[i].push_back(1.0f);

			finalVerticesWalls[i].push_back(wallVertices[wallTriangles[(i * 3) + 1]].x);
			finalVerticesWalls[i].push_back(wallVertices[wallTriangles[(i * 3) + 1]].y);
			finalVerticesWalls[i].push_back(wallVertices[wallTriangles[(i * 3) + 1]].z);
			finalVerticesWalls[i].push_back(1.0f);
			finalVerticesWalls[i].push_back(0.0f);

			finalVerticesWalls[i].push_back(wallVertices[wallTriangles[(i * 3) + 2]].x);
			finalVerticesWalls[i].push_back(wallVertices[wallTriangles[(i * 3) + 2]].y);
			finalVerticesWalls[i].push_back(wallVertices[wallTriangles[(i * 3) + 2]].z);
			finalVerticesWalls[i].push_back(0.0f);
			finalVerticesWalls[i].push_back(1.0f);
		} 
	} // Creates the vertex arrays that opengl can use to generate the cave system.

	void GenerateMesh(std::vector<std::vector<int>> map, float squareSize) 
	{

		triangleDictionary.clear();
		outlines.clear();
		checkedVertices.clear();

		squareGrid = SquareGrid(map, squareSize);

		vertices = std::vector<glm::vec3>();
		triangles = std::vector<int>();

		for (unsigned int x = 0; x < squareGrid.squares.size(); x++) 
		{
			for (int y = 0; y < squareGrid.squares[x].size(); y++)
			{
				TriangulateSquare(squareGrid.squares[x][y]);
			}
		}

		CreateWallMesh();
	} // Firstly clears the dictionary, outline vector, and checked vertices set. Next it will check each individual square for its configuration before creating the final wall vectors.

	void CreateWallMesh() 
	{
		CalculateMeshOutlines();

		wallVertices = std::vector<glm::vec3>();
		wallTriangles = std::vector<int>();
		float wallHeight = 5;

		for (unsigned int j = 0; j < outlines.size(); j++) 
		{
			for (int i = 0; i < outlines[j].size() - 1; i++) 
			{
				int startIndex = wallVertices.size();
				wallVertices.push_back(vertices[outlines[j][i]]); // left
				wallVertices.push_back(vertices[outlines[j][i + 1]]); // right
				wallVertices.push_back(vertices[outlines[j][i]] - glm::vec3(0.0f, 1.0f, 0.0f) * wallHeight); // bottom left
				wallVertices.push_back(vertices[outlines[j][i + 1]] - glm::vec3(0.0f, 1.0f, 0.0f) * wallHeight); // bottom right

				wallTriangles.push_back(startIndex + 0);
				wallTriangles.push_back(startIndex + 2);
				wallTriangles.push_back(startIndex + 3);

				wallTriangles.push_back(startIndex + 3);
				wallTriangles.push_back(startIndex + 1);
				wallTriangles.push_back(startIndex + 0);
			}
		}
	} // Creates the vertices vector and triangle vectors that are needed to create the final vectors that opengl needs.

	void TriangulateSquare(Square square) 
	{
		switch (square.configuration)
		{
		case 0:
			break;

			// 1 points:
		case 1:
			MeshFromPoints(std::vector<Node>() = { square.centerLeft, square.centerBottom, square.bottomLeft });
			break;
		case 2:
			MeshFromPoints(std::vector<Node>() = { square.bottomRight, square.centerBottom, square.centerRight });
			break;
		case 4:
			MeshFromPoints(std::vector<Node>() = { square.topRight, square.centerRight, square.centerTop });
			break;
		case 8:
			MeshFromPoints(std::vector<Node>() = { square.topLeft, square.centerTop, square.centerLeft });
			break;

			// 2 points:
		case 3:
			MeshFromPoints(std::vector<Node>() = { square.centerRight, square.bottomRight, square.bottomLeft, square.centerLeft });
			break;
		case 6:
			MeshFromPoints(std::vector<Node>() = { square.centerTop, square.topRight, square.bottomRight, square.centerBottom });
			break;
		case 9:
			MeshFromPoints(std::vector<Node>() = { square.topLeft, square.centerTop, square.centerBottom, square.bottomLeft });
			break;
		case 12:
			MeshFromPoints(std::vector<Node>() = { square.topLeft, square.topRight, square.centerRight, square.centerLeft });
			break;
		case 5:
			MeshFromPoints(std::vector<Node>() = { square.centerTop, square.topRight, square.centerRight, square.centerBottom, square.bottomLeft, square.centerLeft });
			break;
		case 10:
			MeshFromPoints(std::vector<Node>() = { square.topLeft, square.centerTop, square.centerRight, square.bottomRight, square.centerBottom, square.centerLeft });
			break;

			// 3 point:
		case 7:
			MeshFromPoints(std::vector<Node>() = { square.centerTop, square.topRight, square.bottomRight, square.bottomLeft, square.centerLeft });
			break;
		case 11:
			MeshFromPoints(std::vector<Node>() = { square.topLeft, square.centerTop, square.centerRight, square.bottomRight, square.bottomLeft });
			break;
		case 13:
			MeshFromPoints(std::vector<Node>() = { square.topLeft, square.topRight, square.centerRight, square.centerBottom, square.bottomLeft });
			break;
		case 14:
			MeshFromPoints(std::vector<Node>() = { square.topLeft, square.topRight, square.bottomRight, square.centerBottom, square.centerLeft });
			break;

			// 4 point:
		case 15:
			MeshFromPoints(std::vector<Node>() = { square.topLeft, square.topRight, square.bottomRight, square.bottomLeft });
			checkedVertices.insert(square.topLeft.vertexIndex);
			checkedVertices.insert(square.topRight.vertexIndex);
			checkedVertices.insert(square.bottomRight.vertexIndex);
			checkedVertices.insert(square.bottomLeft.vertexIndex);
			break; // I can insert all these squares vertex index into the set as I know this configuration doesn't have any walls at any side. 
		}
	} // Takes a square and depending on its configuration it will send the appropriate points to the next function. 

	void MeshFromPoints(std::vector<Node>& points)
	{
		AssignVertices(points);

		if (points.size() >= 3)
		{
			CreateTriangle(points[0], points[1], points[2]);
		}
		if (points.size() >= 4)
		{
			CreateTriangle(points[0], points[2], points[3]);
		}
		if (points.size() >= 5)
		{
			CreateTriangle(points[0], points[3], points[4]);
		}
		if (points.size() >= 6)
		{
			CreateTriangle(points[0], points[4], points[5]);
		}
	} // Creates triangles and add the triangle to the map as appropriate. 

	void AssignVertices(std::vector<Node>& points)
	{
		for (int i = 0; i < points.size(); i++) 
		{
			if (points[i].vertexIndex == -1) 
			{
				points[i].vertexIndex = vertices.size();
				vertices.push_back(points[i].position);
			}
		}
	} // Updates the vertexIndex with the neew size of the vertices array, this will be useful later when I need to correlate between these indices and the actual vector3s. Also pushes
	  // the position of the node to the vertices array. 

	void CreateTriangle(Node a, Node b, Node c) 
	{
		triangles.push_back(a.vertexIndex);
		triangles.push_back(b.vertexIndex);
		triangles.push_back(c.vertexIndex);

		Triangle triangle(a.vertexIndex, b.vertexIndex, c.vertexIndex);
		AddTriangleToDictionary(triangle.vertexIndexA, triangle);
		AddTriangleToDictionary(triangle.vertexIndexB, triangle);
		AddTriangleToDictionary(triangle.vertexIndexC, triangle);
	} // Pushes all the vertex index information the a vector we can use later and creates a triangle we push to a map.

	void AddTriangleToDictionary(int& vertexIndexKey, Triangle& triangle) 
	{
		if (triangleDictionary.count(vertexIndexKey) >= 1)
		{
			triangleDictionary[vertexIndexKey].push_back(triangle);
		}
		else 
		{
			std::vector<Triangle> triangleList = std::vector<Triangle>();
			triangleList.push_back(triangle);
			triangleDictionary.insert(std::pair<int, std::vector<Triangle>>(vertexIndexKey, triangleList));
		}
	} // If the vertex index has already been pushed to the triangle list, I push this new triangle, otherwise I add a new triangle list to the dictionary.

	void CalculateMeshOutlines()
	{
		for (int vertexIndex = 0; vertexIndex < vertices.size(); vertexIndex++) 
		{
			if (checkedVertices.find(vertexIndex) == checkedVertices.end())
			{
				int newOutlineVertex = GetConnectedOutlineVertex(vertexIndex);
				if (newOutlineVertex != -1) {
					checkedVertices.insert(vertexIndex);

					std::vector<int> newOutline = std::vector<int>();
					newOutline.push_back(vertexIndex);
					outlines.push_back(newOutline);
					FollowOutline(newOutlineVertex, outlines.size() - 1);
					outlines[outlines.size() - 1].push_back(vertexIndex);
				}
			}
		}
	} // Runs through every vertex in the map and checks if an element is an outline vertex. If it is then it follows the outline 
	  // all the way around until it meets up with itself, then it adds itself to the outline list.

	void FollowOutline(int& vertexIndex, int outlineIndex) 
	{
		outlines[outlineIndex].push_back(vertexIndex);
		checkedVertices.insert(vertexIndex);
		int nextVertexIndex = GetConnectedOutlineVertex(vertexIndex);

		if (nextVertexIndex != -1) 
		{
			FollowOutline(nextVertexIndex, outlineIndex);
		}
	} // Follows the outline until there is no more vertices left. Adds each index to the outline vector as it goes. 

	int GetConnectedOutlineVertex(int& vertexIndex) 
	{
		std::vector<Triangle> trianglesContainingVertex = triangleDictionary[vertexIndex];

		for (int i = 0; i < trianglesContainingVertex.size(); i++) 
		{
			Triangle triangle = trianglesContainingVertex[i];

			for (int j = 0; j < 3; j++) 
			{
				int vertexB = triangle.vertices[j];
				if (vertexB != vertexIndex && (checkedVertices.find(vertexB) == checkedVertices.end())) 
				{
					if (IsOutlineEdge(vertexIndex, vertexB)) 
					{
						return vertexB;
					}
				}
			}
		}

		return -1;
	} // Given an outline vertex, finds a connected vertex which forms the outline edge.

	bool IsOutlineEdge(int& vertexA, int& vertexB) 
	{
		std::vector<Triangle> trianglesContainingVertexA = triangleDictionary[vertexA];
		int sharedTriangleCount = 0;

		for (int i = 0; i < trianglesContainingVertexA.size(); i++) 
		{
			if (trianglesContainingVertexA[i].Contains(vertexB)) 
			{
				sharedTriangleCount++;
				if (sharedTriangleCount > 1) 
				{
					break;
				}
			}
		}
		return sharedTriangleCount == 1;
	} // Given to vertex indices, whether the edge formed by these two vertices is an outline edge or not.
	  // Gets a list of triangles that vertex A belongs to and calculates how many of those triangles 
	  // contains vertex B. If vertex A and B only share one common triangle, that means that it is 
	  // an outline edge
};

#endif
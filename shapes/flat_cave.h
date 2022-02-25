#ifndef FLAT_CAVE_CLASS
#define FLAT_CAVE_CLASS

#include "../buffers/VAO.h"
#include "../buffers/VBO.h"
#include "../texture.h"
#include "../shader.h"
#include <vector>

class FlatCave
{
public:
	std::vector<std::vector<GLfloat>> vertices;
	std::vector<Texture> textures;
	VAO vertexArray;
	bool wireFrame;
	GLsizeiptr stride;

	FlatCave(std::vector<std::vector<GLfloat>>& vertices, bool wireFrame)
	{
		FlatCave::vertices = vertices;
		FlatCave::textures = std::vector<Texture>();
		FlatCave::vertexArray = VAO();
		FlatCave::wireFrame = wireFrame;
		FlatCave::stride = 6 * sizeof(GLfloat);

		VBO vertexBuffer(vertices, (3 * stride));

		vertexArray.Bind();

		vertexArray.LinkAttrib(vertexBuffer, 0, 3, GL_FLOAT, stride, (void*)((3 * stride)));
		vertexArray.LinkAttrib(vertexBuffer, 1, 3, GL_FLOAT, stride, (void*)(((3 * stride)) + (3 * sizeof(GLfloat))));

		vertexArray.Unbind();
		vertexBuffer.Unbind();
	} // Constructor takes the vertices and created the appropriate VBO and VAO objects.

	FlatCave(std::vector<std::vector<GLfloat>>& vertices, bool wireFrame, std::vector<Texture>& textures) 
	{
		FlatCave::vertices = vertices;
		FlatCave::textures = textures;
		FlatCave::vertexArray = VAO();
		FlatCave::wireFrame = wireFrame;
		FlatCave::stride = 5 * sizeof(GLfloat);

		VBO vertexBuffer(vertices, (3 * stride));
		vertexArray = VAO();

		vertexArray.Bind();

		vertexArray.LinkAttrib(vertexBuffer, 0, 3, GL_FLOAT, stride, (void*)((3 * stride)));
		vertexArray.LinkAttrib(vertexBuffer, 1, 2, GL_FLOAT, stride, (void*)((3 * stride) + (3 * sizeof(GLfloat))));

		vertexArray.Unbind();
		vertexBuffer.Unbind();
	} // This version of the constructor takes a vector of textures for the textured part of the cave. 

	void Draw(Shader& shader)
	{

		shader.Use();
		vertexArray.Bind();
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			textures[i].Bind();
		}
		if (wireFrame)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * stride);	
	}

	void Delete() 
	{
		vertexArray.Delete();
	}
};

#endif
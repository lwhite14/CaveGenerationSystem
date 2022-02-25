#ifndef TRIANGLE_CLASS
#define TRIANGLE_CLASS

#include "../buffers/VAO.h"
#include "../buffers/VBO.h"
#include "../texture.h"
#include "../shader.h"
#include <vector>

class ShapeTriangle 
{
public:
	std::vector<GLfloat> vertices;
	std::vector<Texture> textures;
	VAO vertexArray;

	ShapeTriangle(std::vector<GLfloat>& vertices, std::vector<Texture>& textures)
	{
		ShapeTriangle::vertices = vertices;
		ShapeTriangle::textures = textures;

		vertexArray.Bind();

		VBO vertexBuffer(vertices);

		vertexArray.LinkAttrib(vertexBuffer, 0, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)0);
		vertexArray.LinkAttrib(vertexBuffer, 1, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		vertexArray.Unbind();
		vertexBuffer.Unbind();
	}

	ShapeTriangle(std::vector<GLfloat>& vertices)
	{
		ShapeTriangle::vertices = vertices;
		ShapeTriangle::textures = std::vector<Texture>();

		vertexArray.Bind();

		VBO vertexBuffer(vertices);

		vertexArray.LinkAttrib(vertexBuffer, 0, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)0);
		vertexArray.LinkAttrib(vertexBuffer, 1, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		vertexArray.Unbind();
		vertexBuffer.Unbind();
	}

	void Draw(Shader& shader) 
	{
		shader.Use();
		vertexArray.Bind();

		if (textures.size() != 0) 
		{
			for (unsigned int i = 0; i < textures.size(); i++)
			{
				textures[i].Bind();
			}
		}

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
		glDrawArrays(GL_TRIANGLES, 0, 3);	
	}
};

#endif 


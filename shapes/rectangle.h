#ifndef RECTANGLE_CLASS
#define RECTANGLE_CLASS

#include "../buffers/VAO.h"
#include "../buffers/VBO.h"
#include "../buffers/EBO.h"
#include "../texture.h"
#include "../shader.h"
#include <vector>

class ShapeRectangle 
{
public:
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	VAO vertexArray;

	unsigned int instancing;


	ShapeRectangle(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures, unsigned int instancing = 1, std::vector<glm::vec3> instanceVecs = {})
	{
		ShapeRectangle::vertices = vertices;
		ShapeRectangle::indices = indices;
		ShapeRectangle::textures = textures;
		ShapeRectangle::instancing = instancing;

		vertexArray.Bind();

		VBO vertexBuffer(vertices);
		VBO instanceVertexBuffer(instanceVecs);
		EBO elementBuffer(indices);

		
		vertexArray.LinkAttrib(vertexBuffer, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
		vertexArray.LinkAttrib(vertexBuffer, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		if (instancing != 1)
		{
			instanceVertexBuffer.Bind();

			vertexArray.LinkAttrib(instanceVertexBuffer, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
		
			glVertexAttribDivisor(2, 1);
		}

		vertexArray.Unbind();
		vertexBuffer.Unbind();
		instanceVertexBuffer.Unbind();
		elementBuffer.Unbind();
	}

	void Draw(Shader& shader)
	{
		shader.Use();
		vertexArray.Bind();

		for (unsigned int i = 0; i < textures.size(); i++) 
		{
			textures[i].Bind();
		}

		if (instancing == 1) 
		{
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		else 
		{
			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instancing);
		}
	}

};


#endif


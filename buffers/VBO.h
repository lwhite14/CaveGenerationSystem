#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class VBO
{
public:
	GLuint ID;

	VBO(std::vector<GLfloat> vertices)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	} // Buffer for GLfloats, used for object vertices positions.

	VBO(std::vector<glm::vec3> vec3s)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), vec3s.data(), GL_STATIC_DRAW);
	} // Buffer for vector3s, used for instancing object positions.

	VBO(std::vector<std::vector<GLfloat>> vertices, GLsizeiptr stride)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, stride * vertices.size(), 0, GL_STATIC_DRAW);
		for (unsigned int i = 0; i < vertices.size(); i++) 
		{
			glBufferSubData(GL_ARRAY_BUFFER, i * stride, stride, vertices[i].data());
		}

	} // Buffer for storing multiple object vertex positions. 

	void Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}

	void Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Delete()
	{
		glDeleteBuffers(1, &ID);
	}
};

#endif
#ifndef TEXTURE_CLASS
#define TEXTURE_CLASS

#include <GL/glew.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture 
{
public:
	GLuint ID, unit;

	Texture(std::string filePath, GLenum format, GLuint slot) 
	{
		unit = slot;

		glGenTextures(1, &ID);

		glBindTexture(GL_TEXTURE_2D, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);

		unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

		if (data) 
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else 
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	void Bind()
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

};

#endif
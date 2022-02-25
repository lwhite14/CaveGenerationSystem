#ifndef CAMERA_CLASS
#define CAMERA_CLASS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include "motion.h"

class Camera 
{
public:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	float deltaTime;
	float lastFrame;

	bool firstMouse;
	float yaw;
	float pitch;
	float lastX;
	float lastY;

	Motion motion;

	bool isActive;

	Camera(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT) 
	{
		isActive = true;

		deltaTime = 0.0f;
		lastFrame = 0.0f;

		firstMouse = true;
		yaw = -90.0f;
		pitch = 0.0f;
		lastX = SCR_WIDTH / 2.0;
		lastY = SCR_HEIGHT / 2.0;

		motion = { false,false,false,false };

		cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	void UpdateDeltaTime() 
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	void KeyCallback(GLFWwindow* window, int key, int action)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}
		if ((key == GLFW_KEY_GRAVE_ACCENT || key == GLFW_KEY_F1) && action == GLFW_PRESS)
		{
			if (isActive) 
			{
				isActive = false;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else 
			{
				isActive = true;
				firstMouse = true;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}
		if (isActive) 
		{
			if (key == GLFW_KEY_W && action == GLFW_PRESS)
			{
				motion.Forward = true;
			}
			if (key == GLFW_KEY_S && action == GLFW_PRESS)
			{
				motion.Backward = true;
			}
			if (key == GLFW_KEY_A && action == GLFW_PRESS)
			{
				motion.Left = true;
			}
			if (key == GLFW_KEY_D && action == GLFW_PRESS)
			{
				motion.Right = true;
			}

			if (key == GLFW_KEY_W && action == GLFW_RELEASE)
			{
				motion.Forward = false;
			}
			if (key == GLFW_KEY_S && action == GLFW_RELEASE)
			{
				motion.Backward = false;
			}
			if (key == GLFW_KEY_A && action == GLFW_RELEASE)
			{
				motion.Left = false;
			}
			if (key == GLFW_KEY_D && action == GLFW_RELEASE)
			{
				motion.Right = false;
			}
		}
	}

	void Movement()
	{
		float cameraSpeed = 20.0f * deltaTime;
		//glm::vec3 frontAdjusted = glm::vec3(cameraFront.x, 0, cameraFront.z); 
		if (motion.Forward)
		{
			cameraPos += cameraSpeed * cameraFront;
		}
		if (motion.Backward)
		{
			cameraPos -= cameraSpeed * cameraFront;
		}
		if (motion.Left)
		{
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (motion.Right)
		{
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
	}

	void MouseCallback(double xpos, double ypos)
	{
		if (isActive) 
		{
			if (firstMouse)
			{
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
			}

			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			lastX = xpos;
			lastY = ypos;

			float sensitivity = 0.1f;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 89.0f)
			{
				pitch = 89.0f;
			}
			if (pitch < -89.0f)
			{
				pitch = -89.0f;
			}

			glm::vec3 front;
			front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			front.y = sin(glm::radians(pitch));
			front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			cameraFront = glm::normalize(front);
		}
	}

	glm::mat4 ViewLookAt(glm::mat4 inputView) 
	{
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
};


#endif 

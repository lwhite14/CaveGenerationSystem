#include <iostream>
#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "packages/imgui/imgui.h"
#include "packages/imgui/imgui_impl_glfw_gl3.h"

#include "motion.h"
#include "shader.h"
#include "buffers/VAO.h"
#include "buffers/VBO.h"
#include "buffers/EBO.h"
#include "texture.h"
#include "shapes/flat_cave.h"
#include "generation/cave_generator.h"
#include "generation/mesh_generator.h"
#include "camera.h"

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

GLFWwindow* window;
Camera camera(SCR_WIDTH, SCR_HEIGHT);

std::vector<std::vector<GLfloat>> verticesWalls;
std::vector<std::vector<GLfloat>> verticesFloor;

bool wireframeMode[1] = { false };
bool flatMode[1] = { false };
int inputWidth[1] = { 32 };
int inputHeight[1] = { 64 };
float inputFillPercentage[1] = { 0.42f };
char inputSeed[11] = { "" };

int currentSeed = 0;

void CaveGenerationInit(int width, int height, int fillPercentage, int seed);
void GenerateButton(FlatCave& walls, FlatCave& ceiling, FlatCave& floor);
void Debug(FlatCave& walls, FlatCave& ceiling, FlatCave& floor);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
	glfwInit();
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cave Generation System", NULL, NULL);

	glfwMakeContextCurrent(window);
	glewInit();
	 
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MouseCallback);

	glEnable(GL_DEPTH_TEST);

	CaveGenerationInit(32, 64, 42, -1);

	Shader triangleShader("media/shaders/triangle.vert", "media/shaders/triangle.frag");
	Shader triangleTexturedShader("media/shaders/triangleTextured.vert", "media/shaders/triangleTextured.frag");

	FlatCave caveWalls(verticesWalls, false, std::vector<Texture>() = { Texture("media/textures/rock.jpg", GL_RGB, 1) });
	FlatCave caveCeiling(verticesFloor, false);
	FlatCave caveFloor(verticesFloor, false);

	glfwMakeContextCurrent(window);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window))
	{
		camera.UpdateDeltaTime();
		camera.Movement();

		ImGui_ImplGlfwGL3_NewFrame();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Debug(caveWalls, caveCeiling, caveFloor);

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 fullTransformMatrix = glm::mat4(1.0f);

		view = camera.ViewLookAt(view);	
		model = glm::translate(model, glm::vec3(-32.0f, -10.0f, -40.0f));
		fullTransformMatrix = projection * view * model;

		glm::mat4 modelFloor = glm::mat4(1.0f);
		modelFloor = glm::translate(modelFloor, glm::vec3(0.0f, -5.0f, 0.0f));

		triangleShader.Use();
		triangleShader.setMat4("fullTransformMatrix", fullTransformMatrix);

		triangleTexturedShader.Use();
		triangleTexturedShader.setMat4("fullTransformMatrix", fullTransformMatrix);
		triangleTexturedShader.SetInt("sampleTexture", 0);

		if (!flatMode[0]) 
		{
			caveWalls.Draw(triangleTexturedShader);
		}
		caveCeiling.Draw(triangleShader);

		triangleShader.setMat4("fullTransformMatrix", fullTransformMatrix * modelFloor);
		if (!flatMode[0]) 
		{
			caveFloor.Draw(triangleShader);
		}

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void CaveGenerationInit(int width, int height, int fillPercentage, int seed)
{
	verticesWalls = std::vector<std::vector<GLfloat>>();
	verticesFloor = std::vector<std::vector<GLfloat>>();


	CaveGenerator caveGenerator(width, height, fillPercentage, seed);
	MeshGenerator meshGenerator(caveGenerator.borderedMap, 1);

	currentSeed = caveGenerator.seed;

	meshGenerator.CreateFinalVerticesLists(verticesFloor, verticesWalls);
}

void GenerateButton(FlatCave& walls, FlatCave& ceiling, FlatCave& floor)
{
	int fillPercentage = int(inputFillPercentage[0] * 100);
	if (strlen(inputSeed) == 0)
	{
		CaveGenerationInit(inputWidth[0], inputHeight[0], fillPercentage, -1);
	}
	else
	{
		std::stringstream str(inputSeed);
		int newSeed;
		str >> newSeed;
		CaveGenerationInit(inputWidth[0], inputHeight[0], fillPercentage, newSeed);
	}
	walls.Delete();
	ceiling.Delete();
	floor.Delete();

	walls = FlatCave(verticesWalls, wireframeMode[0], std::vector<Texture>() = { Texture("media/textures/rock.jpg", GL_RGB, 1) });
	ceiling = FlatCave(verticesFloor, wireframeMode[0]);
	floor = FlatCave(verticesFloor, wireframeMode[0]);
}

void Debug(FlatCave& walls, FlatCave& ceiling, FlatCave& floor)
{
	ImGui::Begin("Debug");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("Current Seed: %d", currentSeed);
	if (ImGui::Checkbox("Wireframe", wireframeMode))
	{
		walls.wireFrame = wireframeMode[0];
		ceiling.wireFrame = wireframeMode[0];
		floor.wireFrame = wireframeMode[0];
	}
	ImGui::Checkbox("Flat mode", flatMode);
	ImGui::InputInt("X Width ", inputWidth);
	ImGui::InputInt("Z Width ", inputHeight);
	ImGui::InputFloat("Fill Percentage ", inputFillPercentage, 0.01f, 0.01f, 2);
	ImGui::InputText("Seed ", inputSeed, 11); ImGui::SameLine();
	if (ImGui::Button("Reset"))
	{
		memset(&inputSeed[0], 0, sizeof(inputSeed));
	}
	if (ImGui::Button("Generate Cave"))
	{
		GenerateButton(walls, ceiling, floor);
	}
	ImGui::Text("Press ` or F1 to toggle to debug menu");
	ImGui::Text("Press ESC to exit");
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(400, 260));
	ImGui::End();
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) 
{
	camera.MouseCallback(xpos, ypos);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	camera.KeyCallback(window, key, action);
}

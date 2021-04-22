#include <Windows.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>

#include "GLRenderingProgram.h"

#define PI 3.14159

enum class SPECIES { ONE = 1, TWO, ALL };

unsigned constexpr WIDTH = 1920;
unsigned constexpr HEIGHT = 1080;
unsigned constexpr SLIME_AMOUNT = 100000;
SPECIES constexpr SPECIES_NUM = SPECIES::ALL;

extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

struct Slime
{
	glm::vec2 pos;
	float angle;
	unsigned int colorMask;
};

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to init GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Slime Simulator", nullptr, nullptr);

	char* errStr;
	int errorCode = glfwGetError((const char**)&errStr);
	if (errorCode != GLFW_NO_ERROR)
	{
		std::cerr << errStr << std::endl;
		return errorCode;
	}

	glfwMakeContextCurrent(window);

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cerr << glewGetErrorString(glewError) << std::endl;
		return glewError;
	}

	glfwSwapInterval(1);

	GLRenderingProgram prog = GLRenderingProgram("vert.glsl", "frag.glsl");
	GLRenderingProgram slimeProg = GLRenderingProgram("slimeCompute.glsl");
	GLRenderingProgram fadeProg = GLRenderingProgram("fadeComp.glsl");

	float positions[] = {
		0.0f, HEIGHT,
		WIDTH, 0.0f,
		0.0f, 0.0f,

		0.0f, HEIGHT,
		WIDTH, HEIGHT,
		WIDTH, 0.0f
	};

	GLuint vao;
	GLuint vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glm::vec3 cameraLoc = glm::vec3(0.0f, 0.0f, -1.0f);

	glm::mat4 proj = glm::ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), cameraLoc);

	glm::mat4 matrix = proj * view;

	glFrontFace(GL_CCW);

	//Create and Bind Texture
	GLuint tex;
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glClearTexImage(tex, 0, GL_RGBA, GL_FLOAT, nullptr);

	//Create slimes
	Slime* slimes = new Slime[SLIME_AMOUNT];
	srand(time(NULL));

	for (unsigned i = 0; i < SLIME_AMOUNT; i++)
	{
		slimes[i].pos = glm::vec2(rand() % WIDTH, rand() % HEIGHT);
		slimes[i].angle = (rand() % 360) * (PI / 180.0f);
		slimes[i].colorMask = rand() % (int)SPECIES_NUM ;
	}

	GLuint ssbo[2];
	glGenBuffers(2, ssbo);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Slime) * SLIME_AMOUNT, slimes, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo[0]);

	float frameTime = 0.0f;
	
	while (!glfwWindowShouldClose(window))
	{
		float startTime = glfwGetTime();

		//Run Compute Shaders
		//Fade shader
		fadeProg.use();
		glDispatchCompute(WIDTH / 30, HEIGHT / 30, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		//Slime sim
		slimeProg.use();
		glUniform1f(slimeProg.getUniformLoc("time"), frameTime);

		glDispatchCompute(SLIME_AMOUNT / 1000, 1, 1);

		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		//Run Render Shader
		prog.use();

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(prog.getUniformLoc("matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);

		glDrawArrays(GL_TRIANGLES, 0, sizeof(positions) / sizeof(positions[0]));
		glfwSwapBuffers(window);

		glfwPollEvents();

		frameTime = glfwGetTime() - startTime;
	}

	delete[] slimes;

	return 0;
}

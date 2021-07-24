#include <Windows.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <vector>

#include "GLRenderingProgram.h"
#include "OpenGLFailure.h"

#define PI 3.14159

//Disable debug console for release
#ifdef _DEBUG
#define MAIN main()
#else
#define MAIN WinMain(HINSTANCE hIstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#endif

enum class SPECIES { ONE = 1, TWO, ALL };

unsigned constexpr WIDTH = 1920;
unsigned constexpr HEIGHT = 1080;
unsigned constexpr SLIME_AMOUNT = 100000;
SPECIES constexpr SPECIES_NUM = SPECIES::ALL;
unsigned constexpr SIM_CALLS = SLIME_AMOUNT / 1000;

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

int MAIN
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

	GLRenderingProgram* prog;
	GLRenderingProgram* slimeProg;
	GLRenderingProgram* fadeProg;
	GLRenderingProgram* sortProg;

	try
	{
		prog      = new GLRenderingProgram("vert.glsl", "frag.glsl");
		slimeProg = new GLRenderingProgram("slimeCompute.glsl");
		fadeProg  = new GLRenderingProgram("fadeComp.glsl");
		sortProg  = new GLRenderingProgram("SorterComp.glsl");
	}
	catch (const OpenGLFailure& er)
	{
		std::cerr << er.what() << std::endl;

#ifdef _DEBUG
		std::cin.ignore();
#endif

		return -1;
	}

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
		slimes[i].pos = glm::vec2(HEIGHT / 4.0f * cos(i * 2 * PI / SLIME_AMOUNT) + WIDTH / 2.0f, HEIGHT / 4.0f * sin(i * 2 * PI / SLIME_AMOUNT) + HEIGHT / 2.0f);
		slimes[i].angle = tan(slimes[i].pos.y / slimes[i].pos.x);
		slimes[i].colorMask = rand() % (int)SPECIES_NUM ;
	}

	GLuint ssbo[2];
	glGenBuffers(2, ssbo);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Slime) * SLIME_AMOUNT, slimes, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo[0]);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Slime) * SLIME_AMOUNT, nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo[1]);

	float frameTime = 0.0f;
	uint32_t sorted = false;
	
	while (!glfwWindowShouldClose(window))
	{
		float startTime = glfwGetTime();

		//Run Compute Shaders
		//Fade shader
		fadeProg->use();
		glDispatchCompute(WIDTH / 30, HEIGHT / 30, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//Slime sim
		slimeProg->use();
		glUniform1f(slimeProg->getUniformLoc("time"), frameTime);
		glDispatchCompute(SIM_CALLS, 1, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		//Sorter
		//sortProg.use();
		//glDispatchCompute(SIM_CALLS, 1, 1);

		//Run Render Shader
		prog->use();

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(prog->getUniformLoc("matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

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

	delete prog;
	delete slimeProg;
	delete fadeProg;
	delete sortProg;

	return 0;
}

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include <iostream>
#include <Windows.h>

#include "GLRenderingProgram.h"

unsigned constexpr WIDTH = 720;
unsigned constexpr HEIGHT = 480;

extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void APIENTRY glDebugOutput(GLenum src, GLenum type, unsigned id, GLenum severity,
	GLsizei length, const char* msg, const void* userParam);

int main()
{
	if (!glfwInit())
		return - 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Triangle", nullptr, nullptr);

	char* erStr;
	int glfwEr = glfwGetError((const char**)&erStr);
	if (glfwEr != GLFW_NO_ERROR)
	{
		std::cerr << erStr << std::endl;
		return glfwEr;
	}

	glfwMakeContextCurrent(window);

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cerr << glewGetErrorString(glewError) << std::endl;
		return glewError;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	glfwSwapInterval(1);

	GLRenderingProgram prog("vert.glsl", "frag.glsl");

	float positions[] {
		100.0f, 100.0f,
		0.0f, 000.0f,
		200.0f, 0.0f,
	};

	GLuint vao;
	GLuint vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glm::mat4 pMat = glm::ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT);

	glm::vec3 cameraLoc(0.0f, 0.0f, -1.0f);
	glm::mat4 vMat = glm::translate(glm::mat4(1.0f), cameraLoc);

	glFrontFace(GL_CCW);

	while (!glfwWindowShouldClose(window))
	{
		prog.use();

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		GLuint pLoc = glGetUniformLocation(prog.getID(), "proj");
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(pMat));

		GLuint vLoc = glGetUniformLocation(prog.getID(), "view");
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		int size = sizeof(positions) / sizeof(positions[0]) / 2;
		glDrawArrays(GL_TRIANGLES, 0, size);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	
}

void APIENTRY glDebugOutput(GLenum src, GLenum type, unsigned id,
	GLenum severity, GLsizei length, const char * msg, const void * userParam)
{
	/*if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
		return;*/

	std::cerr << msg << std::endl;

	std::cerr << std::endl << std::endl;

	switch (src)
	{
	case GL_DEBUG_SOURCE_API:
		std::cerr << "Source: API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		std::cerr << "Source: Window System";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		std::cerr << "Source: Shader Compiler";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		std::cerr << "Source: Third Party";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		std::cerr << "Source: Application";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		std::cerr << "Source: Other";
		break;
	}

	std::cerr << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		std::cerr << "Type: Error";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cerr << "Type: Deprecated Behavior";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cerr << "Type: Undefined Behavior";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		std::cerr << "Type: Portability";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cerr << "Type: Performance";
		break;

	case GL_DEBUG_TYPE_MARKER:
		std::cerr << "Type: Marker";
		break;

	case GL_DEBUG_TYPE_PUSH_GROUP:
		std::cerr << "Type: Push Group";
		break;

	case GL_DEBUG_TYPE_POP_GROUP:
		std::cerr << "Type: Pop Group";
		break;

	case GL_DEBUG_TYPE_OTHER:
		std::cerr << "Type: Other";
		break;
	}

	std::cerr << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		std::cerr << "Severity: High";
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cerr << "Severity: Medium";
		break;

	case GL_DEBUG_SEVERITY_LOW:
		std::cerr << "Severity: Low";
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cerr << "Severity: Notification";
		break;
	}

	std::cerr << std::endl << "-------------------------------------------" << std::endl;
}

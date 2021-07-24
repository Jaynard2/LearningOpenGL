#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <iostream>
#include <Windows.h>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLRenderingProgram.h"
#include "OpenGLFailure.h"

extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

unsigned constexpr WIDTH = 2048u;
unsigned constexpr HEIGHT = 1080u;

glm::vec4 sides(-2.5f, 1.0f, -1.0f, 1.0f);
glm::vec3 cameraLoc;
glm::mat4 proj;
glm::mat4 view;
glm::mat4 mat;
GLuint accuracy = 512;
float negate = 0.0f;

glm::vec4 oldPos;
bool mousePressed = false;

double zoomLevel = 1.0f;

void scrollCallback(GLFWwindow* window, double x, double y);
void mouseCallback(GLFWwindow* window, int button, int action, int mods);
std::ostream& operator<<(std::ostream& out, const glm::mat4& matrix);

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Could not initiliaze GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Mandlebrot Set", nullptr, nullptr);

	char* glfwErr;
	int errorCode = glfwGetError((const char**)&glfwErr);
	if (errorCode != GLFW_NO_ERROR)
	{
		std::cerr << "GLFW Error: " << glfwErr << std::endl;
		return errorCode;
	}

	glfwSetScrollCallback(window, scrollCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);

	glfwMakeContextCurrent(window);

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cerr << "GLEW failed to initialize\n";
		return glewError;
	}

	glfwSwapInterval(1);

	GLRenderingProgram* render;
	try
	{
		render = new GLRenderingProgram("vert.glsl", "frag.glsl");
	}
	catch (const OpenGLFailure& er)
	{
		std::cerr << er.what() << std::endl;
		return -1;
	}

	render->use();

	float positions[] = {
		-3.0f, 3.0f,
		3.0f, -3.0f,
		-3.0f, -3.0f,

		-3.0f, 3.0f,
		3.0f, 3.0f,
		3.0f, -3.0f
	};

	glFrontFace(GL_CCW);

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);
	
	cameraLoc = glm::vec3(0.0f, 0.0f, -1.0f);
	proj = glm::ortho(sides.x, sides.y, sides.z, sides.w);
	view = glm::translate(glm::mat4(1.0f), cameraLoc);
	mat = proj * view;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		if (mousePressed)
		{
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			mouseY *= -1;

			glm::vec4 pos = glm::vec4((float)mouseX / (WIDTH / 2.0f) - 1.0f, (float)mouseY / (HEIGHT / 2.0f) + 1.0f, -1.0f, 1.0f);
			pos = glm::inverse(mat) * pos;
			pos /= pos.w;
			pos.z = 0.0f;

			cameraLoc += glm::vec3(pos - oldPos);
			view = glm::translate(glm::mat4(1.0f), cameraLoc);
			mat = proj * view;
		}

		glUniformMatrix4fv(render->getUniformLoc("mat"), 1, GL_FALSE, glm::value_ptr(mat));
		glUniform1f(render->getUniformLoc("negative"), negate);
		glUniform1ui(render->getUniformLoc("accuracy"), accuracy);

		glDrawArrays(GL_TRIANGLES, 0, sizeof(positions) / sizeof(positions[0]));


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete render;
}

void scrollCallback(GLFWwindow* window, double x, double y)
{
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	mouseY *= -1;

	glm::vec4 posPreZoom = glm::vec4(mouseX / (WIDTH / 2.0f) - 1, mouseY / (HEIGHT / 2.0f) + 1, -1.0f, 1.0f);
	posPreZoom = glm::inverse(mat) * posPreZoom;
	posPreZoom /= posPreZoom.w;

	if (y > 0.0f)
		zoomLevel *= 1.10f;
	else
		zoomLevel *= 0.905f;
	
	proj = glm::ortho(sides.x / (float)zoomLevel, sides.y / (float)zoomLevel, sides.z / (float)zoomLevel, sides.w / (float)zoomLevel);
	mat = proj * view;

	glm::vec4 posPostZoom = glm::vec4(mouseX / (WIDTH / 2.0f) - 1, mouseY / (HEIGHT / 2.0f) + 1, -1.0f, 1.0f);
	posPostZoom = glm::inverse(mat) * posPostZoom;
	posPostZoom /= posPostZoom.w;

	cameraLoc += glm::vec3(posPostZoom - posPreZoom);
	view = glm::translate(glm::mat4(1.0f), cameraLoc);
	mat = proj * view;
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			mouseY *= -1;

			oldPos = glm::vec4((float)mouseX / (WIDTH / 2.0f) - 1, (float)mouseY / (HEIGHT / 2.0f) + 1, -1.0f, 1.0f);
			oldPos = glm::inverse(mat) * oldPos;
			oldPos /= oldPos.w;
			oldPos.z = 0.0f;

			mousePressed = true;
		}
		else
			mousePressed = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		if (negate == 0.0f)
		{
			negate = 1.0f;
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			negate = 0.0f;
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_4 && action == GLFW_PRESS && accuracy < 4096)
		accuracy *= 2;
	else if (button == GLFW_MOUSE_BUTTON_5 && action == GLFW_PRESS && accuracy > 1)
		accuracy /= 2;
}

std::ostream& operator<<(std::ostream& out, const glm::mat4& matrix)
{
	for (unsigned i = 0; i < matrix.length(); i++)
	{
		if (i == 0)
			std::cout << '[';

		for (unsigned j = 0; j < matrix[i].length(); j++)
		{
			std::cout << matrix[i][j] << ' ';
		}

		if (i != matrix.length() - 1)
			std::cout << '\n';
		else
			std::cout << ']';
	}

	return out;
}

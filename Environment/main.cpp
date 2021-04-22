#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include "Utilities.h"
#include "Torus.h"

#define numVAOs 1
#define numVBOs 4

GLuint renderingProgram, skyboxProg;
GLuint skyboxTex;
GLuint vLoc, projLoc, mvLoc, nLoc;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
Torus myTorus;

glm::vec3 cameraLoc(0.0f, 0.0f, 0.0f);
glm::vec3 torusLoc(0.0f, 0.5f, -2.0f);

unsigned windowX = 1920, windowY = 1080;
unsigned numCubeVertices;

void init(GLFWwindow* window);
void display(GLFWwindow* window, double currentTime);
void resizeWindowCallback(GLFWwindow* window, int width, int height);
void setupVertices();

int main()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	GLFWwindow* window = glfwCreateWindow(windowX, windowY, "Chapter 2 - Program 1", NULL, NULL);

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	glfwSwapInterval(1);

	init(window);

	setupVertices();

	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

void init(GLFWwindow * window)
{
	//glfwSetWindowMonitor(window, NULL, -2500, 600, windowX, windowY, 0);
	glfwSetWindowSize(window, windowX, windowY);
	resizeWindowCallback(window, windowX, windowY);

	glfwSetWindowSizeCallback(window, resizeWindowCallback);

	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	skyboxProg = Utils::createShaderProgram("vertSkybox.glsl", "fragSkybox.glsl");

	skyboxTex = Utils::loadCubeMap("../Cubemaps+Skydomes/LakeIslands/cubeMap");
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

}

void display(GLFWwindow * window, double currentTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	vMat = glm::translate(glm::mat4(1.0), glm::vec3(-cameraLoc.x, -cameraLoc.y, -cameraLoc.z));

	//Skybox
	glUseProgram(skyboxProg);

	vLoc = glGetUniformLocation(skyboxProg, "v_matrix");
	projLoc = glGetUniformLocation(skyboxProg, "proj_matrix");

	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glDisable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLES, 0, numCubeVertices);

	glEnable(GL_DEPTH_TEST);

	//World Objects
	glUseProgram(renderingProgram);

	mMat = glm::translate(glm::mat4(1.0f), torusLoc);
	mMat = glm::rotate(mMat, glm::pi<float>() / 4.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, myTorus.getNumIndices(), GL_UNSIGNED_INT, 0);

}

void setupVertices()
{
	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	std::vector<float> cube = {
	-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
	1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
	1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};

	numCubeVertices = cube.size();

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, numCubeVertices * sizeof(cube[0]), &cube[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, myTorus.getNumVertices() * sizeof(myTorus.getVertices()[0]),
		&myTorus.getVertices()[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, myTorus.getNumIndices() * sizeof(myTorus.getIndices()[0]),
		&myTorus.getIndices()[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, myTorus.getNormals().size() * sizeof(myTorus.getNormals()[0]),
		&myTorus.getNormals()[0], GL_STATIC_DRAW);
}

void resizeWindowCallback(GLFWwindow* window, int width, int height)
{
	glfwGetFramebufferSize(window, &width, &height);
	float aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Windows.h>
#include <stack>
#include <iostream>

#include "Utilities.h"
#include "Sphere.h"
#include "Torus.h"
#include "ImportedModel.h"

//Select NVIDIA grphics card
extern "C" 
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

#define numVAOs 1
#define numVBOs 9

#define PI 3.14159f

float scrollPos = 0.96f;

ImportedModel pyramid("../Models+Textures/otherModels/pyr.obj");
Torus myTorus(0.6f, 0.4f, 48);
Sphere lightSrc;
int numPyramidVertices, numTorusVertices, numTorusIndices, numCubeVertices, numCubeNormals;

glm::vec3 torusLoc(1.6f, 0.0f, -0.3f);
glm::vec3 pyrLoc(-1.0f, 0.1f, 0.3f);
glm::vec3 cameraLoc(0.0f, 0.2f, 6.0f);
glm::vec3 lightLoc(-3.8f, 2.2f, 1.1f);

GLuint shadowProgram, drawProgram, skyboxProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint mvLoc, projLoc, nLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc, sLoc;
GLuint skyboxTexture;

glm::mat4 pMat, vMat, invTrMat, mMat, mvMat;

glm::vec3 currentLightPos, lightPosV;
float lightPos[3];

//White light properties
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

//Used in display() for transfering light to shaders
float curAmb[4], curDif[4], curSpe[4], matAmb[4], matDif[4], matSpe[4];
float curShi, matShi;

//shadow variables
int screenSizeX, screenSizeY;
GLuint shadowTex, shadowBuffer;
glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP1;
glm::mat4 shadowMVP2;
glm::mat4 b;

void init(GLFWwindow* window);
void display(GLFWwindow* window, double currentTime);
void installLights(GLuint renderingProgram, glm::mat4 vMatrix);
void setupShadowBuffers(GLFWwindow* window);

void passOne();
void passTwo();

void setupVertices();
void resizeWindowCallback(GLFWwindow* window, int width, int height);

void scrollCallback(GLFWwindow* window, double x, double y);

int main()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Chapter 2 - Program 1", NULL, NULL);
	//glfwSetWindowMonitor(window, NULL, -2500, 600, 1920, 1080, 0);
	//glfwSetWindowSize(window, 1920, 1080);

	char* errorString;
	int glfwError = glfwGetError((const char**)&errorString);
	if (glfwError != 0)
	{
		std::cout << errorString << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cout << glewGetErrorString(glewError) << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, resizeWindowCallback);

	init(window);

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
	shadowProgram = Utils::createShaderProgram("vertShadow.glsl", "fragShadow.glsl");
	drawProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	skyboxProgram = Utils::createShaderProgram("SkyboxVert.glsl", "skyboxFrag.glsl");

	glfwSetScrollCallback(window, scrollCallback);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	resizeWindowCallback(window, width, height);

	currentLightPos = lightLoc;

	setupVertices();
	setupShadowBuffers(window);

	b = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	skyboxTexture = Utils::loadCubeMap("../Cubemaps+Skydomes/LakeIslands/cubeMap");
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

}

void display(GLFWwindow * window, double currentTime)
{	
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	//render cube map
	glUseProgram(skyboxProgram);

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraLoc.x, -cameraLoc.y, -cameraLoc.z));
	
	GLuint vLoc = glGetUniformLocation(skyboxProgram, "v_matrix");
	projLoc = glGetUniformLocation(skyboxProgram, "proj_matrix");

	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//make cubemap active texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	//disable depth testing and draw map
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glEnable(GL_DEPTH_TEST);

	//Lights
	//Get mouse pos
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	yPos *= -1;

	//Transform mouse from screen sapce to world space
	glm::mat4 inverse = glm::inverse(pMat * vMat);
	glm::vec4 pos = glm::vec4(xPos / (1920.0f / 2) - 1, yPos / (1080.0f / 2) + 1, scrollPos, 1.0f);
	pos = inverse * pos;
	pos /= pos.w;

	currentLightPos = pos;

	lightVmatrix = glm::lookAt(currentLightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightPmatrix = glm::perspective(60.0f * PI / 180.0f, (float)screenSizeX / (float)screenSizeY, 0.1f, 1000.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);

	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);	// for reducing
	glPolygonOffset(2.0f, 4.0f);		//  shadow artifacts

	passOne();

	glDisable(GL_POLYGON_OFFSET_FILL);	// artifact reduction, continued

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glDrawBuffer(GL_FRONT);

	passTwo();

}

void passOne()
{
	glUseProgram(shadowProgram);

	mMat = glm::translate(glm::mat4(1.0f), torusLoc);
	mMat = glm::rotate(mMat, 25.0f * PI / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	//Drawing from lights POV; use P and V matrices
	shadowMVP1 = lightPmatrix * lightVmatrix *mMat;
	sLoc = glGetUniformLocation(shadowProgram, "shadowMVP");
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	//Only need torus vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);

	//Repeat for pyramid w/o clearing depth buffer.
	//Pyramid not indexed, so use glDrawArrays

	mMat = glm::translate(glm::mat4(1.0f), pyrLoc);
	mMat = glm::rotate(mMat, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mMat = glm::rotate(mMat, glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	shadowMVP1 = lightPmatrix * lightVmatrix *mMat;

	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	
	glDrawArrays(GL_TRIANGLES, 0, numPyramidVertices);
}

void passTwo()
{
	glUseProgram(drawProgram);

	//Need to use MVP for both camera and light space
	mvLoc = glGetUniformLocation(drawProgram, "mv_matrix");
	projLoc = glGetUniformLocation(drawProgram, "proj_matrix");
	nLoc = glGetUniformLocation(drawProgram, "norm_matrix");
	sLoc = glGetUniformLocation(drawProgram, "shadowMVP2");

	//Torus is bronze
	curAmb[0] = Utils::bronzeAmbient()[0]; curAmb[1] = Utils::bronzeAmbient()[1]; curAmb[2] = Utils::bronzeAmbient()[2];
	curDif[0] = Utils::bronzeDiffuse()[0]; curDif[1] = Utils::bronzeDiffuse()[1]; curDif[2] = Utils::bronzeDiffuse()[2];
	curSpe[0] = Utils::bronzeSpecular()[0]; curSpe[1] = Utils::bronzeSpecular()[1]; curSpe[2] = Utils::bronzeSpecular()[2];
	curShi = Utils::bronzeShininess();

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraLoc.x, -cameraLoc.y, -cameraLoc.z));

	installLights(drawProgram, vMat);

	mMat = glm::translate(glm::mat4(1.0f), torusLoc);
	mMat = glm::rotate(mMat, 25.0f * PI / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

	//put mv and proj matrices into uniforms
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

	//setup torus vertices and normal buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //torus verts
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); //torus norms
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]); //torus indices
	glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);

	//Pyramid is gold
	curAmb[0] = Utils::goldAmbient()[0]; curAmb[1] = Utils::goldAmbient()[1]; curAmb[2] = Utils::goldAmbient()[2];
	curDif[0] = Utils::goldDiffuse()[0]; curDif[1] = Utils::goldDiffuse()[1]; curDif[2] = Utils::goldDiffuse()[2];
	curSpe[0] = Utils::goldSpecular()[0]; curSpe[1] = Utils::goldSpecular()[1]; curSpe[2] = Utils::goldSpecular()[2];
	curShi = Utils::goldShininess();

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraLoc.x, -cameraLoc.y, -cameraLoc.z));

	installLights(drawProgram, vMat);

	mMat = glm::translate(glm::mat4(1.0f), pyrLoc);
	mMat = glm::rotate(mMat, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mMat = glm::rotate(mMat, glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

	//put mv and proj matrices into uniforms
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

	//setup pyramid vertices and normal buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]); //pyramid verts
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]); //pyramid norms
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, numPyramidVertices);

	//Draw lightSrc
	mMat = glm::translate(glm::mat4(1.0f), currentLightPos);
	mMat = glm::scale(mMat, glm::vec3(0.25f, 0.25f, 0.25f));

	installLights(drawProgram, vMat);

	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	//put mv and proj matrices into uniforms
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

	//setup lightSrc vertices and normal buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]); //lightSrc verts
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]); //lightSrc norms
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[7]);
	glDrawElements(GL_TRIANGLES, lightSrc.getNumIndices(), GL_UNSIGNED_INT, 0);

}

void installLights(GLuint renderingProgram, glm::mat4 vMatrix)
{
	//convert light's pos to view space, and save it to float array
	lightPosV = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0f));

	lightPos[0] = lightPosV.x;
	lightPos[1] = lightPosV.y;
	lightPos[2] = lightPosV.z;

	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	mAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram, mAmbLoc, 1, curAmb);
	glProgramUniform4fv(renderingProgram, mDiffLoc, 1, curDif);
	glProgramUniform4fv(renderingProgram, mSpecLoc, 1, curSpe);
	glProgramUniform1f(renderingProgram, mShiLoc, curShi);
}

void setupShadowBuffers(GLFWwindow * window)
{
	glfwGetFramebufferSize(window, &screenSizeX, &screenSizeY);

	glGenFramebuffers(1, &shadowBuffer);

	//create shadow buffer and configure it to hold depth info
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, screenSizeX, screenSizeY,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void setupVertices()
{
	numTorusIndices = myTorus.getNumIndices();
	numTorusVertices = myTorus.getNumVertices();

	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, numTorusVertices * sizeof(myTorus.getVertices()[0]),
		&myTorus.getVertices()[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, myTorus.getNormals().size() * sizeof(myTorus.getNormals()[0]),
		&myTorus.getNormals()[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numTorusIndices * sizeof(myTorus.getIndices()[0]),
		&myTorus.getIndices()[0], GL_STATIC_DRAW);

	numPyramidVertices = pyramid.getNumVertices();

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, numPyramidVertices * sizeof(pyramid.getVertices()[0]),
		&pyramid.getVertices()[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, pyramid.getNormals().size() * sizeof(pyramid.getNormals()[0]),
		&pyramid.getNormals()[0], GL_STATIC_DRAW);

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

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, cube.size() * sizeof(cube[0]), &cube[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, lightSrc.getNumVertices() * sizeof(lightSrc.getVertices()[0]),
		&lightSrc.getVertices()[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[7]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lightSrc.getNumIndices() * sizeof(lightSrc.getIndices()[0]),
		&lightSrc.getIndices()[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glBufferData(GL_ARRAY_BUFFER, lightSrc.getNormals().size() * sizeof(lightSrc.getNormals()[0]),
		&lightSrc.getNormals()[0], GL_STATIC_DRAW);
}

void resizeWindowCallback(GLFWwindow* window, int width, int height)
{
	glfwGetFramebufferSize(window, &width, &height);
	float aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

void scrollCallback(GLFWwindow * window, double x, double y)
{
	float offset = 0.001f;

	if (y > 0)
		scrollPos += offset;
	else
		scrollPos -= offset;

	if (scrollPos > 1.0f)
		scrollPos = 1.0f;
	else if (scrollPos < 0.0f)
		scrollPos = 0.0f;
}

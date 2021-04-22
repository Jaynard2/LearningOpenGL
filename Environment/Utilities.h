#pragma once
#include <GL/glew.h>
#include <string>

namespace Utils
{
	GLuint createShaderProgram(const char* vert, const char* frag);
	std::string readShaderSource(std::string path);

	void printShaderLog(GLuint shader);
	void printProgramLog(int prog);
	bool checkOpenGLError();
	GLuint loadTexture(const char* texImagePath);
	GLuint loadCubeMap(const char* mapDir);

	//Gold
	float* goldAmbient();
	float* goldDiffuse();
	float* goldSpecular();
	float goldShininess();

	//Silver
	float* silverAmbient();
	float* silverDiffuse();
	float* silverSpecular();
	float silverShininess();

	//Bronze
	float* bronzeAmbient();
	float* bronzeDiffuse();
	float* bronzeSpecular();
	float bronzeShininess();
}

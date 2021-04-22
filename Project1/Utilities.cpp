#include "Utilities.h"

#include <fstream>
#include <iostream>
#include <SOIL2/SOIL2.h>

namespace Utils
{
	GLuint loadCubeMap(const char* mapDir)
	{
		GLuint textureRef;

		std::string xp = mapDir + std::string("/xp.jpg");
		std::string xn = mapDir + std::string("/xn.jpg");
		std::string yp = mapDir + std::string("/yp.jpg");
		std::string yn = mapDir + std::string("/yn.jpg");
		std::string zp = mapDir + std::string("/zp.jpg");
		std::string zn = mapDir + std::string("/zn.jpg");

		textureRef = SOIL_load_OGL_cubemap(xp.c_str(), xn.c_str(), yp.c_str(), yn.c_str(), zp.c_str(), zn.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

		if (textureRef == 0)
			std::cout << "Cube map images not found\n";

		glBindTexture(GL_TEXTURE_CUBE_MAP, textureRef);

		//reduce seams
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureRef;
	}

	GLuint createShaderProgram(const char* vert, const char* frag)
	{
		std::string vShaderStr = readShaderSource(vert);
		std::string fShaderStr = readShaderSource(frag);

		const char* vshaderSource = vShaderStr.c_str();
		const char* fshaderSource = fShaderStr.c_str();

		GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vShader, 1, &vshaderSource, NULL);
		glShaderSource(fShader, 1, &fshaderSource, NULL);


		GLint vertCompiled;
		glCompileShader(vShader);
		checkOpenGLError();
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
		if (vertCompiled != 1)
		{
			std::cout << "Vertex Shader Compilation Failed" << std::endl;
			printShaderLog(vShader);
		}

		GLint fragCompiled;
		glCompileShader(fShader);
		checkOpenGLError();
		glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
		if (fragCompiled != 1)
		{
			std::cout << "Fragment Shader Compilation Failed" << std::endl;
			printShaderLog(fShader);
		}

		GLuint vfProgram = glCreateProgram();
		glAttachShader(vfProgram, vShader);
		glAttachShader(vfProgram, fShader);

		GLint linked;
		glLinkProgram(vfProgram);
		checkOpenGLError();
		glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
		if (linked != 1)
		{
			std::cout << "Linking Failed" << std::endl;
			printProgramLog(vfProgram);
		}

		return vfProgram;

	}

	std::string readShaderSource(std::string path)
	{
		std::string shader = "";
		std::ifstream file(path);

		if (!file.is_open())
			return std::string();

		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);
			shader += line + '\n';
		}

		file.close();

		return shader;
	}

	void printShaderLog(GLuint shader)
	{
		int len = 0;
		int chWrittn = 0;
		char* log;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			log = new char[len];
			log[len - 1] = '\0';
			glGetShaderInfoLog(shader, len, &chWrittn, log);

			std::cout << "Shader Info Log: " << log << std::endl;

			delete[] log;
		}
	}

	void printProgramLog(int prog)
	{
		int len = 0;
		int chWrittn = 0;
		char* log;

		glGetShaderiv(prog, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			log = new char[len];
			log[len - 1] = '\0';
			glGetProgramInfoLog(prog, len, &chWrittn, log);

			std::cout << "Program Info Log: " << log << std::endl;

			delete[] log;
		}
	}

	bool checkOpenGLError()
	{
		bool foundError = false;

		int glErr = glGetError();
		while (glErr != GL_NO_ERROR)
		{
			std::cout << "glError: " << glErr << std::endl;
			foundError = true;

			glErr = glGetError();
		}

		return foundError;
	}

	GLuint loadTexture(const char * texImagePath)
	{
		GLuint textureID = SOIL_load_OGL_texture(texImagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
		{
			GLfloat anisoSetting = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoSetting);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoSetting);
		}

		if (textureID == 0)
			std::cout << "Could not find texture file " << texImagePath << std::endl;

		return textureID;
	}

	float * goldAmbient()
	{
		static float a[4] = { 0.2473f, 0.1995f, 0.0745f, 1 };
		return a;
	}

	float * goldDiffuse()
	{
		static float a[4] = { 0.7516f, 0.6065f, 0.2265f, 1 };
		return a;
	}

	float * goldSpecular()
	{
		static float a[4] = { 0.6283f, 0.5559f, 0.3661f, 1 };
		return a;
	}

	float goldShininess()
	{
		return 51.2f;
	}

	float * silverAmbient()
	{
		static float a[4] = { 0.1923f, 0.1923f , 0.1923f, 1 };
		return a;
	}

	float * silverDiffuse()
	{
		static float a[4] = { 0.5075f, 0.5075f, 0.5075f, 1 };
		return a;
	}

	float * silverSpecular()
	{
		static float a[4] = { 0.5083, 0.5083, 0.5083, 1 };
		return a;
	}

	float silverShininess()
	{
		return 51.2f;
	}

	float * bronzeAmbient()
	{
		static float a[4] = { 0.2125f, 0.1275f, 0.0540f, 1 };
		return a;
	}

	float * bronzeDiffuse()
	{
		static float a[4] = { 0.7140f, 0.4284f, 0.1814f, 1 };
		return a;
	}

	float * bronzeSpecular()
	{
		static float a[4] = { 0.3936f, 0.2719f, 0.1667f, 1 };
		return a;
	}

	float bronzeShininess()
	{
		return 25.6f;
	}
}

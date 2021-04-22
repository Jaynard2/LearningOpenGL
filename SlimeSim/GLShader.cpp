#include "GLShader.h"
#include <fstream>

#include "OpenGLFailure.h"

GLShader::GLShader() : _shader(0), _initialized(false), _errStr("")
{
}

GLShader::GLShader(std::string shader, ShaderType type) : _shader(0), _initialized(false), _errStr("")
{
	create(shader, type);
}

void GLShader::create(std::string shader, ShaderType type)
{
	_shader = glCreateShader((GLenum)type);

	std::ifstream shaderContent(shader);
	if (shaderContent.fail())
		throw std::ios_base::failure("Failed to open file " + shader);

	std::string data;
	while (!shaderContent.eof())
	{
		std::string temp;
		std::getline(shaderContent, temp);

		data.append(temp + '\n');
	}

	const GLchar* glData = data.c_str();

	glShaderSource(_shader, 1, &glData, 0);

	GLint status;
	glCompileShader(_shader);

	checkOpenGLError();
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		saveLog();
		throw OpenGLFailure(_errStr);
	}

	_initialized = true;
}

const GLuint & GLShader::getID() const
{
	if (_shader == 0)
		throw OpenGLFailure("Using unintialized shader");

	return _shader;
}

void GLShader::checkOpenGLError()
{

	int glErr = glGetError();
	if (glErr != GL_NO_ERROR)
	{
		throw OpenGLFailure("Failed with OpenGL error " + glErr);
	}
}

void GLShader::saveLog()
{
	int len = 0;
	int chWrittn = 0;
	char* log;

	glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = new char[len];
		log[len - 1] = '\0';
		glGetShaderInfoLog(_shader, len, &chWrittn, log);

		_errStr = log;

		delete[] log;
	}
}

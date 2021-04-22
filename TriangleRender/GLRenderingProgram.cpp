#include "GLRenderingProgram.h"

#include "OpenGLFailure.h"

GLRenderingProgram::GLRenderingProgram(const std::string & vertexShader, const std::string & fragShader)
{
	_vShader = std::make_shared<GLShader>(vertexShader, ShaderType::VERTEX_SHADER);
	_fShader = std::make_shared<GLShader>(fragShader, ShaderType::FRAGMENT_SHADER);

	init();
}

GLRenderingProgram::GLRenderingProgram(std::shared_ptr<GLShader> vShader, std::shared_ptr<GLShader> fShader) :
	_vShader(_vShader), _fShader(fShader)
{
	if (!_vShader->isInitialized() || !_fShader->isInitialized())
		throw OpenGLFailure("Cannot create program with unintialized shaders");

	init();
}

void GLRenderingProgram::use() const
{
	glUseProgram(_prog);
}

void GLRenderingProgram::init()
{
	_prog = glCreateProgram();
	glAttachShader(_prog, _vShader->getID());
	glAttachShader(_prog, _fShader->getID());

	GLint status;
	glLinkProgram(_prog);

	checkOpenGLError();
	glGetProgramiv(_prog, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		saveProgramLog();
		throw OpenGLFailure(_errLog);
	}

}

void GLRenderingProgram::checkOpenGLError()
{

	int glErr = glGetError();
	if (glErr != GL_NO_ERROR)
	{
		throw OpenGLFailure("Failed with OpenGL error " + glErr);
	}
}

void GLRenderingProgram::saveProgramLog()
{
	int len = 0;
	int chWrittn = 0;
	char* log;

	glGetProgramiv(_prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = new char[len];
		log[len - 1] = '\0';
		glGetProgramInfoLog(_prog, len, &chWrittn, log);

		_errLog = log;

		delete[] log;
	}
}

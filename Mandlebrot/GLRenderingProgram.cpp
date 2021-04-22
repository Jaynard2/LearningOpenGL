#include "GLRenderingProgram.h"

#include "OpenGLFailure.h"

GLRenderingProgram::GLRenderingProgram(const std::string & vertexShader, const std::string & fragShader)
{
	_vShader = std::make_shared<GLShader>(vertexShader, ShaderType::VERTEX_SHADER);
	_fShader = std::make_shared<GLShader>(fragShader, ShaderType::FRAGMENT_SHADER);

	init(false);
}

GLRenderingProgram::GLRenderingProgram(std::shared_ptr<GLShader> vShader, std::shared_ptr<GLShader> fShader) : _vShader(_vShader), _fShader(fShader)
{
	if (!_vShader->isInitialized() || !_fShader->isInitialized())
		throw OpenGLFailure("Cannot create program with unintialized shaders");

	init(false);
}

GLRenderingProgram::GLRenderingProgram(const std::string & cShader)
{
	_cShader = std::make_shared<GLShader>(cShader, ShaderType::COMPUTE_SHADER);

	init(true);
}

GLRenderingProgram::GLRenderingProgram(std::shared_ptr<GLShader> cShader) : _cShader(cShader)
{
	if (!_cShader->isInitialized())
		throw OpenGLFailure("Cannot create program with unitialized shader");

	init(true);
}

void GLRenderingProgram::use() const
{
	glUseProgram(_prog);
}

const GLint & GLRenderingProgram::getUniformLoc(std::string uniformName) const
{
	return glGetUniformLocation(_prog, uniformName.c_str());
}

void GLRenderingProgram::init(bool computeShader)
{
	_prog = glCreateProgram();
	if (!computeShader)
	{
		glAttachShader(_prog, _vShader->getID());
		glAttachShader(_prog, _fShader->getID());
	}
	else
		glAttachShader(_prog, _cShader->getID());

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

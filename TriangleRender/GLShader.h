#pragma once
#include <GL/glew.h>
#include <string>

enum class ShaderType { VERTEX_SHADER = GL_VERTEX_SHADER, FRAGMENT_SHADER = GL_FRAGMENT_SHADER };

class GLShader
{
public:
	GLShader();
	GLShader(std::string shader, ShaderType type);

	void create(std::string shader, ShaderType type);

	const GLuint& getID() const;
	bool isInitialized() const { return _initialized; }

private:
	GLuint _shader;
	bool _initialized;
	std::string _errStr;

	void checkOpenGLError();
	void saveLog();
};

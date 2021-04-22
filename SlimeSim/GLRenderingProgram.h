#pragma once
#include <string>
#include <GL/glew.h>
#include <memory>

#include "GLShader.h"

class GLRenderingProgram
{
public:
	GLRenderingProgram(const std::string& vertexShader, const std::string& fragShader);
	GLRenderingProgram(std::shared_ptr<GLShader> vShader, std::shared_ptr<GLShader> fShader);

	GLRenderingProgram(const std::string& cShader);
	GLRenderingProgram(std::shared_ptr<GLShader> cShader);

	void use() const;

	std::shared_ptr<GLShader> getVertexShader() const { return _vShader; }
	std::shared_ptr<GLShader> getFragmentShader() const { return _fShader; }
	const GLint& getID() const { return _prog; }
	const GLint& getUniformLoc(std::string uniformName) const;

private:
	std::shared_ptr<GLShader> _vShader;
	std::shared_ptr<GLShader> _fShader;
	std::shared_ptr<GLShader> _cShader;
	GLint _prog;

	std::string _errLog;

	void init(bool computeShader);
	void checkOpenGLError();
	void saveProgramLog();

};

#pragma once
#include <stdexcept>
#include <string>

class OpenGLFailure : public std::runtime_error
{
public:
	OpenGLFailure(std::string what = "");
};


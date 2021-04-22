#include "OpenGLFailure.h"

OpenGLFailure::OpenGLFailure(std::string what) : std::runtime_error(what)
{
}

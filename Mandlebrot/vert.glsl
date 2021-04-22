#version 430
layout (location = 0) in vec2 verts;
uniform mat4 mat;

out vec2 pos;

void main()
{
	gl_Position = mat * vec4(verts, 0.0f, 1.0f);
	pos = verts;
}

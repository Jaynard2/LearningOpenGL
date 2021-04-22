#version 430
layout (location = 0) in vec2 vert;

uniform mat4 proj;
uniform mat4 view;

void main()
{
	gl_Position = proj * view * vec4(vert, 0.0, 1.0);
}

#version 430
layout (location = 0) in vec2 pos;
layout(binding = 0) uniform sampler2D tex;

out vec2 tc;

uniform mat4 matrix;

void main()
{
	gl_Position = matrix * vec4(pos, 0.0, 1.0);

	ivec2 size = textureSize(tex, 0);

	tc = vec2(pos.x / size.x, pos.y / size.y);
}

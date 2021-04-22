#version 430
layout(binding = 0) uniform sampler2D tex;

in vec2 tc;
layout(location = 0) out vec4 color;

void main()
{
	color = texture(tex, tc);
}

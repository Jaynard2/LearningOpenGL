#version 430
layout (location = 0) in vec3 position;
out vec3 tc;

uniform mat4 v_matrix;
uniform mat4 proj_matrix;
layout (binding = 0) uniform samplerCube samp;

void main()
{
	tc = position;
	mat4 vrot_matrix = mat4(mat3(v_matrix)); //remove translation
	gl_Position = proj_matrix * vrot_matrix * vec4(position, 1.0);
}

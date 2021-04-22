#version 430
layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;

out vec3 varyingNormal;
out vec3 varyingLightDir;
out vec3 varyingVertPos;
out vec3 halfVector;
out vec4 shadow_coord;

struct PositionalLight
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};

struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
uniform mat4 shadowMVP2;
layout (binding = 0) uniform sampler2DShadow shTex;

mat4 buildTranslate(float x, float y, float z);
mat4 buildRotateX(float rad);
mat4 buildRotateY(float rad);
mat4 buildRotateZ(float rad);
mat4 buildScale(float x, float y, float z);


void main(void)
{
	varyingVertPos = (mv_matrix * vec4(vertPos, 1.0)).xyz;
	varyingLightDir = light.position - varyingVertPos;
	varyingNormal = (norm_matrix * vec4(vertNormal, 1.0)).xyz;
	halfVector = (varyingLightDir - varyingVertPos).xyz;
	shadow_coord = shadowMVP2 * vec4(vertPos, 1.0);

	gl_Position = proj_matrix * mv_matrix * vec4(vertPos, 1.0);
}

mat4 buildTranslate(float x, float y, float z)
{
	mat4 trans = mat4(1.0, 0.0, 0.0, 0.0,
					  0.0, 1.0, 0.0, 0.0,
					  0.0, 0.0, 1.0, 0.0,
					  x, y, z, 1.0);

	return trans;
}

mat4 buildRotateX(float rad)
{
	mat4 xrot = mat4(1.0, 0.0, 0.0, 0.0,
					 0.0, cos(rad), -sin(rad), 0.0,
					 0.0, sin(rad), cos(rad), 0.0,
					 0.0, 0.0, 0.0, 1.0);

	return xrot;
}

mat4 buildRotateY(float rad)
{
	mat4 yrot = mat4(cos(rad), 0.0, sin(rad), 0.0,
					 0.0, 1.0, 0.0, 0.0,
					 -sin(rad), 0.0, cos(rad), 0.0,
					 0.0, 0.0, 0.0, 1.0);

	return yrot;
}

mat4 buildRotateZ(float rad)
{
	mat4 zrot = mat4(cos(rad), -sin(rad), 0.0, 0.0,
					 sin(rad), cos(rad), 0.0, 0.0,
					 0.0, 0.0, 1.0, 0.0,
					 0.0, 0.0, 0.0, 1.0);

	return zrot;
}

mat4 buildScale(float x, float y, float z)
{
	mat4 scale = mat4(x, 0.0, 0.0, 0.0,
				 	  0.0, y, 0.0, 0.0,
				 	  0.0, 0.0, z, 0.0,
				 	  0.0, 0.0, 0.0, 1.0);

	return scale; 
}
#version 430
in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec3 halfVector;
in vec4 shadow_coord;

out vec4 fragColor;

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

float lookup(float ox, float oy);

void main(void)
{
	float shadowFactor = 0.0;
	vec3 L = normalize(varyingLightDir);
	vec3 N = normalize(varyingNormal);
	vec3 V = normalize(-varyingVertPos);
	vec3 H = normalize(halfVector);
	
	float swidth = 2.5;

	//4-sample lookup
	vec2 offset = mod(floor(gl_FragCoord.xy), 2.0) * swidth;
	shadowFactor += lookup(-1.5 * swidth + offset.x, 1.5 * swidth - offset.y);
	shadowFactor += lookup(-1.5 * swidth + offset.x, -0.5 * swidth - offset.y);
	shadowFactor += lookup(0.5 * swidth + offset.x, 1.5 * swidth - offset.y);
	shadowFactor += lookup(0.5 * swidth + offset.x, -0.5 * swidth - offset.y);
	shadowFactor /= 4.0;

	vec4 shadowColor = globalAmbient * material.ambient + light.ambient * material.ambient;
	vec4  lightedColor = light.diffuse * material.diffuse * max(dot(L, N), 0.0)
						+ light.specular * material.specular
						* pow(max(dot(H, N), 0.0), material.shininess * 3.0);
	
	fragColor = vec4((shadowColor.xyz + shadowFactor * lightedColor.xyz), 1.0);
}

float lookup(float ox, float oy)
{
	float scaleX = 1 / textureSize(shTex, 0).x;
	float scaleY = 1 / textureSize(shTex, 0).y;
	return textureProj(shTex, shadow_coord + vec4(ox * scaleX * shadow_coord.w, oy * scaleY * shadow_coord.w, -0.01, 0.0));
}

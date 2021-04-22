#version 430
uniform mat4 mat;
uniform float negative;
uniform uint accuracy;
in vec2 pos;
out vec4 color;

uint testPoint(vec2 point);

void main()
{
	float weight = float(testPoint(pos.xy)) / accuracy;
	color = vec4(abs(vec3(int(weight < 0.4f) * weight * 2, int(weight < 0.8f) * weight, int(weight < 0.8f) * weight) - negative), 1.0f);
}

uint testPoint(vec2 point)
{
	vec2 z = vec2(0);
	int i = 0;

	while(z.x * z.x + z.y * z.y < 4.0f && i < accuracy)
	{
		float temp = z.x * z.x - z.y *  z.y;
		z.y = 2 * z.x * z.y;
		z.x = temp;
		z += point;

		i++;
	}
	
	return i;
}

#version 430
layout (local_size_x = 30, local_size_y = 30) in;
layout(rgba32f, binding = 0) uniform image2D tex;

void main()
{
	vec4 sumColor = vec4(0);
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(tex);

	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			sumColor += imageLoad(tex, pos + ivec2(x, y));
		}
	}

	imageStore(tex, pos, (sumColor / 9.0f) * 0.99f);
}

#version 430
layout (local_size_x = 1000) in;
layout(rgba32f, binding = 0) uniform image2D tex;

struct Slime
{
	vec2 pos;
	float angle;
	uint colorMask;
};

layout (binding = 0) buffer slime
{
	
	Slime slimes[];
};
layout (binding = 1) buffer data
{
	Slime slimesCopy[];
};

/*
	1. Determine what grid the slime belongs in
	2. Sort slime based on grid number
*/

const ivec2 size = imageSize(tex);
const ivec2 gridAmount = ivec2(50, 20);
const vec2 gridSize = vec2(size.x / gridAmount.x, size.y / gridAmount.y);
const uint index = gl_GlobalInvocationID.x;

void drawCircle(const vec2 pos, const float radius, const vec4 color);
int getGridID(vec2 pos);

void main()
{
	int currentID = getGridID(slimes[index].pos);
}

int getGridID(vec2 pos)
{
	int x = int(pos.x) / int(gridSize.x);
	int y = int(pos.y) / int(gridSize.y);

	return x + y * gridAmount.x;
}

void drawCircle(const vec2 pos, const float radius, const vec4 color)
{
	for(float x = pos.x - radius; x < pos.x + radius; x++)
	{
		for(float y = pos.y - radius; y < pos.y + radius; y++)
		{
			if(abs(distance(pos, vec2(x, y))) < radius &&
				x >= 0 && x < size.x &&
				y >= 0 && y < size.y)
			{
				imageStore(tex, ivec2(x, y), color);
			}
		}
	}
}

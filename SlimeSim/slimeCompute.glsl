#version 430
#define PI 3.14159

uniform float time;

layout (local_size_x = 1000) in;
layout(rgba32f, binding = 0) uniform image2D tex;

const uint RED = 0u;
const uint GREEN = 1u;
const uint BLUE = 2u;

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

const float slimeSize = 1.0f;
const float speed = 100.0f;  //Speed slime travels
const float turnSpeed = 60.0f;
const float randStrength = 0.99f;
const float lookAngle = PI / 4;
const int	lookSize = 3;
const float lookOffset = 4.0f;      //Distance from slime to check for trail
const ivec2 size = imageSize(tex);

float pickDirection(const Slime sl);
float calcValue(const Slime sl, const float angle);
void drawCircle(const Slime sl, const float radius);
void fadeOld();
float random (const vec2 st);

void main()
{
	uint index = gl_GlobalInvocationID.x;

	drawCircle(slimes[index], slimeSize);

	float angleOfTravel = pickDirection(slimes[index]);
	vec2 delta = vec2(speed * time * cos(angleOfTravel), speed * time * sin(angleOfTravel));

	slimes[index].pos += delta;
	slimes[index].angle = angleOfTravel;
	

	slimes[index].pos.x = int(slimes[index].pos.x <= size.x) * slimes[index].pos.x;
	slimes[index].pos.x = int(slimes[index].pos.x >= 0) * slimes[index].pos.x + int(slimes[index].pos.x < 0) * size.x;

	slimes[index].pos.y = int(slimes[index].pos.y <= size.y) * slimes[index].pos.y;
	slimes[index].pos.y = int(slimes[index].pos.y >= 0) * slimes[index].pos.y + int(slimes[index].pos.y < 0) * size.y;
}

float pickDirection(const Slime sl)
{
	
	float left = calcValue(sl, -lookAngle);
	float forward = calcValue(sl, 0.0f);
	float right = calcValue(sl, lookAngle);

	float rand = random(sl.pos * time);

	if(forward > left && forward > right)
		return sl.angle;
	else if(forward < left && forward < right)
		return sl.angle + (rand - 0.5f) * randStrength * turnSpeed * time;
	else if(left > right)
		return sl.angle - rand * randStrength * turnSpeed * time;

	return sl.angle + rand * randStrength * turnSpeed * time;


}
float calcValue(const Slime sl, const float angle)
{
	vec2 center = vec2(cos(sl.angle + angle), sin(sl.angle + angle)) * lookOffset + sl.pos;

	float sum = 0.0f;
	for(int x = -lookSize; x <= lookSize; x++)
	{
		for(int y = -lookSize; y <= lookSize; y++)
		{
			vec4 color = imageLoad(tex, ivec2(x, y) + ivec2(center));
			sum += color.r * (int(sl.colorMask == RED) - int(sl.colorMask != RED));
			sum += color.g * (int(sl.colorMask == GREEN) - int(sl.colorMask != GREEN));
			sum += color.b * (int(sl.colorMask == BLUE) - int(sl.colorMask != BLUE));
		}
	}

	return sum;
}


void drawCircle(const Slime sl, const float radius)
{
	for(float x = sl.pos.x - radius; x < sl.pos.x + radius; x++)
	{
		for(float y = sl.pos.y - radius; y < sl.pos.y + radius; y++)
		{
			if(abs(distance(sl.pos, vec2(x, y))) < radius &&
				x >= 0 && x < size.x &&
				y >= 0 && y < size.y)
			{
				vec4 color = imageLoad(tex, ivec2(x, y));

				color.r = uint(sl.colorMask == RED) + color.r * int(sl.colorMask != RED);
				color.g = uint(sl.colorMask == GREEN) + color.g * int(sl.colorMask != GREEN);
				color.b = uint(sl.colorMask == BLUE) + color.b * int(sl.colorMask != BLUE);
				
				imageStore(tex, ivec2(x, y), color);
			}
		}
	}
}

float random (const vec2 st) 
{
    return fract(sin(dot(st, vec2(12.9898,78.233))) * 43758.5453123);
}

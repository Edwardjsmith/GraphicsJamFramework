#version 460 core

struct Ray
{
	vec3 Direction;
	vec3 Origin;
};

void BuildRay(inout Ray ray, in vec3 origin, in vec3 direction)
{
	ray.Origin = origin;
	ray.Direction = normalize(direction);
}

vec3 CastRay(in Ray ray, float scalar) { return ray.Origin + (scalar * ray.Direction); }

struct PixelData
{
	vec4 color;
	//vec2 coords;
};

layout(binding = 0) buffer block
{
	PixelData data[];
} outBuffer;

uniform float delta;

uniform vec3 cameraPos;
uniform vec3 cameraLowerLeft;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

uniform int screenWidth = 640;
uniform int screenHeight = 480;

vec3 screenCentre = vec3(screenWidth * 0.5, screenHeight * 0.5, 0);

Ray GetRay(in vec3 pos, in vec3 direction)
{
	Ray ray;

	BuildRay(ray, pos, direction);

	return ray;
}

bool SphereHit(in vec3 pos, float radius, in Ray ray)
{
	vec3 diff = ray.Origin - pos;

	float a = dot(ray.Direction, ray.Direction);
	float b = 2.0 * dot(diff,ray.Direction);
	float c = dot(diff, diff) - (radius * radius);

	float discriminant = b * b - 4 * a * c;

	return discriminant >= 0;
}

vec3 GetRayColor(in Ray ray, uint x, uint y)
{
	//if (SphereHit(vec3(0, 0, 1), 10, ray))
	{
		//return vec3(1, 0, 0);
	}

	float colourX = (x / (screenWidth * 2.0f)) * 255.0f;
	float colourY = (y / (screenHeight * 2.0f)) * 255.0f;

	return vec3(colourX, colourY, 0);

}   

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main()
{
	uint indexX = (gl_WorkGroupID.x * gl_WorkGroupSize.x) + gl_LocalInvocationID.x;
	uint indexY = (gl_WorkGroupID.y * gl_WorkGroupSize.y) + gl_LocalInvocationID.y;

	vec3 pixelCentre = vec3(indexX, indexY, 0);

	vec3 rayDir = screenCentre - pixelCentre;
	Ray ray = GetRay(screenCentre, rayDir);

	outBuffer.data[indexX + (indexY * screenWidth)].color = vec4(GetRayColor(ray, indexX, indexY), 1.0f);
}
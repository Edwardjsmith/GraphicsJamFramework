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

uniform mat4 inverseProjectionView;

uniform int screenWidth = 640;
uniform int screenHeight = 480;

uniform vec3 cameraPos;

vec4 screenCentre = vec4(screenWidth * 0.5, screenHeight * 0.5, 0, 1);

Ray GetRay(in vec3 pos, in vec3 direction)
{
	Ray ray;

	BuildRay(ray, pos, direction);

	return ray;
}

bool SphereHit(in vec3 centre, float minDist, float maxDist, float radius, in Ray ray)
{
	vec3 diff = ray.Origin - centre;
	float a = dot(diff, ray.Direction);

	if (a < 0)
	{
		return false;
	}

	float b = length(diff);

	float c = (a * a) - (b * b);
	float radiusSquared = radius * radius;

	if (c > radiusSquared)
	{
		return false;
	}

	float d = sqrt(radiusSquared - c);

	float pointOne = a - d;

	if (pointOne < minDist || pointOne > maxDist)
	{
		float pointTwo = a + d;

		if (pointTwo < minDist || pointTwo > maxDist)
		{
			return false;
		}
	}

	return true;
}

vec3 GetRayColor(in Ray ray, uint x, uint y)
{
	if (SphereHit(vec3(0, 0, 0), 0.001f, 1000000.0f, 0.5, ray))
	{
		return vec3(0, 0, 255);
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

	vec4 pixelCentre = vec4(indexX, indexY, cameraPos.z, 0);

	vec4 screenDir = screenCentre - pixelCentre;
	
	vec4 rayDir = normalize(vec4(inverseProjectionView * vec4(screenDir)));

	Ray ray = GetRay(cameraPos + pixelCentre.xyz, rayDir.xyz);

	outBuffer.data[indexX + (indexY * screenWidth)].color = vec4(GetRayColor(ray, indexX, indexY), 1.0f);
}
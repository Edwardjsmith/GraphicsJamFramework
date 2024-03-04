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
};

layout(binding = 0) buffer block
{
	PixelData data[];
} outBuffer;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform float delta;

uniform vec3 cameraPos;
uniform vec3 cameraLowerLeft;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

int screenWidth = 640;
int screenHeight = 480;

vec3 viewPortU = vec3(screenWidth, 0, 0);
vec3 viewPortV = vec3(0, -screenHeight, 0);

vec3 pixelU = viewPortU / screenWidth;
vec3 pixelV = viewPortV / screenHeight;

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

vec3 GetRayColor(in Ray ray)
{
	if (SphereHit(vec3(0, 0, -1), 0.5, ray))
	{
		return vec3(1, 0, 0);
	}

	vec3 normalizedDir = normalize(ray.Direction);
	float scalar = 0.5 * (normalizedDir.x + 1.0);

	return (1.0 - scalar) * vec3(1) + scalar * vec3(0.5, 0.7, 1.0);
}   

void main()
{
	for (int i = 0; i < screenHeight; ++i)
	{
		for (int j = 0; j < screenWidth; ++j)
		{
			vec3 viewPortLeft = cameraPos - ((viewPortU * 0.5) - (viewPortV * 0.5));
			vec3 topLeftPixelLoc = viewPortLeft + 0.5 * (pixelU + pixelV);
			vec3 pixelCentre = topLeftPixelLoc + (i * pixelV) + (j * pixelU);

			vec3 rayDir = pixelCentre - cameraPos;
			Ray ray = GetRay(cameraPos, rayDir);

			outBuffer.data[(screenWidth * i) + j].color = vec4(GetRayColor(ray), 0) * 255;
		}
	}
}
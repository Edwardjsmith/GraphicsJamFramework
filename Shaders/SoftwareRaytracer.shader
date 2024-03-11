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

uniform mat4 inverseProjection;
uniform mat4 inverseView;

uniform int screenWidth = 640;
uniform int screenHeight = 480;

uniform vec3 cameraPos;
uniform vec3 cameraForward;

float halfScreenHeight = screenHeight * 0.5f;
float halfScreenWidth = screenWidth * 0.5f;

Ray GetRay(in vec3 pos, in vec3 direction)
{
	Ray ray;

	BuildRay(ray, pos, direction);

	return ray;
}

bool SphereHit(in vec3 centre, float minDist, float maxDist, float radius, in Ray ray)
{
	// work out components of quadratic
	vec3 dist = ray.Origin - centre;
	float b = dot(ray.Direction, dist);
	float c = dot(dist, dist) - radius * radius;
	float b_squared_minus_c = b * b - c;

	// check for "imaginary" answer. == ray completely misses sphere
	if (b_squared_minus_c < 0.0f) 
	{ 
		return false; 
	}

	// check for ray hitting twice (in and out of the sphere)
	if (b_squared_minus_c > 0.0f) 
	{
		// get the 2 intersection distances along ray
		float t_a = -b + sqrt(b_squared_minus_c);
		float t_b = -b - sqrt(b_squared_minus_c);

		// if behind viewer, throw one or both away
		if (t_a < 0.0)
		{
			if (t_b < 0.0) 
			{
				return false; 
			}
		}

		return true;
	}

	// check for ray hitting once (skimming the surface)
	if (0.0f == b_squared_minus_c) 
	{
		// if behind viewer, throw away
		float t = -b + sqrt(b_squared_minus_c);
		if (t < 0.0f) 
		{ 
			return false;
		}

		return true;
	}
}

vec3 GetRayColor(in Ray ray, uint x, uint y)
{
	if (SphereHit(vec3(0, 0, 0), 0.001f, 1000000.0f, 0.5f, ray))
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

	//Convert pixel pos to normalized screen coords (X [-1, 1], Y [-1, 1], Z [-1, 1])
	float xPos = ((2.0f * indexX) / screenWidth) - 1.0f;
	float yPos = (1.0f - (2.0f * indexY) / screenHeight);
	float zPos = -1.0f;

	vec3 rayNDS = vec3(xPos, yPos, zPos);

	//Homogeneous clip coords
	vec4 rayClip = vec4(rayNDS.xyz, 1.0);

	//Camera coords
	vec4 rayCamera = inverseProjection * rayClip;
	rayCamera = vec4(rayCamera.xy, -1.0f, 0.0f);

	//World Coords
	vec3 rayWorld = normalize((inverseView * rayCamera).xyz);

	Ray ray = GetRay(cameraPos, rayWorld);

	outBuffer.data[indexX + (indexY * screenWidth)].color = vec4(GetRayColor(ray, indexX, indexY), 1.0f);
}
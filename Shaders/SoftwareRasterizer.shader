#version 460 core

struct PixelData
{
	vec4 color;
};

struct Triangle
{
	vec3 vertices[3];
	vec2 boxCoords[4];
};

uniform mat4 Projection;
uniform mat4 View;

void BuildTriangle(inout Triangle triangle, in vec3 left, in vec3 right, in vec3 top)
{
	triangle.vertices[0] = left;
	triangle.vertices[1] = right;
	triangle.vertices[2] = top;
}

//void ProjectTriangle(in Triangle triangle, out vec3 left, out vec3 right, out vec3 top)
//{
//	//Project triangle vertices
//	left = (View * Projection * triangle.vertices[0]).xyz;
//	right = (View * Projection * triangle.vertices[1]).xyz;
//	top = (View * Projection * triangle.vertices[2]).xyz;
//
//	//init box coords with projected coords
//	triangle.boxCoords[0] = left.xy;
//	triangle.boxCoords[1] = vec2(left.x, top.y);
//	triangle.boxCoords[2] = vec2(right.x, top.y);
//	triangle.boxCoords[3] = right.xy;
//}

bool edgeFunc(in vec3 a, in vec3 b, in vec3 c)
{
	return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x)) >= 0.0f;
}

bool PixelInsideTraingle(in vec3 left, in vec3 right, in vec3 top, in vec3 pixelCoords)
{
	return edgeFunc(left, right, pixelCoords) && edgeFunc(right, top, pixelCoords) && edgeFunc(top, left, pixelCoords);
}

layout(binding = 0) buffer pixel
{
	PixelData data[];
} pixelDataBuffer;

layout(binding = 1) buffer depth
{
	float data[];
} depthBuffer;

uniform int screenWidth = 640;
uniform int screenHeight = 480;

uniform vec3 cameraPos;

float halfScreenHeight = screenHeight * 0.5f;
float halfScreenWidth = screenWidth * 0.5f;

vec3 ToRaster(in vec3 NDC)
{
	return vec3((NDC.x) * (screenWidth), (NDC.y) * screenHeight, 1.0f);
}

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main()
{
	uint indexX = (gl_WorkGroupID.x * gl_WorkGroupSize.x) + gl_LocalInvocationID.x;
	uint indexY = (gl_WorkGroupID.y * gl_WorkGroupSize.y) + gl_LocalInvocationID.y;

	uint index = indexX + (indexY * screenWidth);
	depthBuffer.data[index] = 10000000.0f;

	vec3 left = ToRaster(vec3(0.25f, 0.75f, 1.0f));
	vec3 right = ToRaster(vec3(0.75f, 0.75f, 1.0f));
	vec3 top = ToRaster(vec3(0.5f, 0.25f, 1.0f));

	if (PixelInsideTraingle(left, right, top, vec3(indexX, indexY, 1)))
	{
		pixelDataBuffer.data[index].color = vec4(0.0f, 0.0f, 255.0f, 1.0f);
	}
	else
	{
		float colourX = (indexX / (screenWidth * 2.0f)) * 255.0f;
		float colourY = (indexY / (screenHeight * 2.0f)) * 255.0f;

		pixelDataBuffer.data[index].color = vec4(vec3(colourX, colourY, 0), 1.0f);
	}

	cameraPos;
	Projection;
	View;

}
#version 460 core

struct PixelData
{
	vec4 color;
	//vec2 screenCoords;
};

struct Triangle
{
	vec4 vertices[3];
	vec2 boxCoords[4];
};

uniform mat4 Projection;
uniform mat4 View;

void BuildTriangle(inout Triangle triangle, in vec3 left, in vec3 right, in vec3 top)
{
	triangle.vertices[0] = vec4(left, 1);
	triangle.vertices[1] = vec4(right, 1);
	triangle.vertices[2] = vec4(top, 1);
}

void ProjectTriangle(in Triangle triangle, out vec3 left, out vec3 right, out vec3 top)
{
	//Project triangle vertices
	left = (View * Projection * triangle.vertices[0]).xyz;
	right = (View * Projection * triangle.vertices[1]).xyz;
	top = (View * Projection * triangle.vertices[2]).xyz;

	//init box coords with projected coords
	triangle.boxCoords[0] = left.xy;
	triangle.boxCoords[1] = vec2(left.x, top.y);
	triangle.boxCoords[2] = vec2(right.x, top.y);
	triangle.boxCoords[3] = right.xy;
}

bool PixelInsideTraingle(in vec3 left, in vec3 right, in vec3 top, vec2 pixelCoords)
{
	return false;
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

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main()
{
	uint indexX = (gl_WorkGroupID.x * gl_WorkGroupSize.x) + gl_LocalInvocationID.x;
	uint indexY = (gl_WorkGroupID.y * gl_WorkGroupSize.y) + gl_LocalInvocationID.y;

	uint index = indexX + (indexY * screenWidth);
	depthBuffer.data[index] = 10000000.0f;

	//Build triangle
	Triangle triangle;
	vec3 triangleLeftCorner = vec3(halfScreenWidth - 10, halfScreenHeight - 10, -10);
	vec3 triangleRightCorner = vec3(halfScreenWidth + 10, halfScreenHeight - 10, -10);
	vec3 triangleTopCorner = vec3(halfScreenWidth, halfScreenHeight + 10, -10);

	BuildTriangle(triangle, triangleLeftCorner, triangleRightCorner, triangleTopCorner);
	ProjectTriangle(triangle, triangleLeftCorner, triangleRightCorner, triangleTopCorner);

	if (PixelInsideTraingle(triangleLeftCorner, triangleRightCorner, triangleTopCorner, vec2(indexX, indexY)))
	{
		pixelDataBuffer.data[index].color = vec4(0.0f, 0.0f, 255.0f, 1.0f);
		//pixelDataBuffer.data[index].screenCoords = vec2(indexX, indexY);
	}
	else
	{
		float colourX = (indexX / (screenWidth * 2.0f)) * 255.0f;
		float colourY = (indexY / (screenHeight * 2.0f)) * 255.0f;

		pixelDataBuffer.data[index].color = vec4(vec3(colourX, colourY, 0), 1.0f);
		//pixelDataBuffer.data[index].screenCoords = vec2(indexX, indexY);
	}

	cameraPos;
	Projection;
	View;

}
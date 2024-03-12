#version 460 core

struct PixelData
{
	vec4 color;
};

layout(binding = 0) buffer block
{
	PixelData data[];
} outBuffer;

uniform int screenWidth = 640;
uniform int screenHeight = 480;

uniform vec3 cameraPos;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main()
{
	uint indexX = (gl_WorkGroupID.x * gl_WorkGroupSize.x) + gl_LocalInvocationID.x;
	uint indexY = (gl_WorkGroupID.y * gl_WorkGroupSize.y) + gl_LocalInvocationID.y;

	cameraPos;

	float colourX = (indexX / (screenWidth * 2.0f)) * 255.0f;
	float colourY = (indexY / (screenHeight * 2.0f)) * 255.0f;


	outBuffer.data[indexX + (indexY * screenWidth)].color = vec4(vec3(colourX, colourY, 0), 1.0f);
}
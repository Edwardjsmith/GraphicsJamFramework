#version 460 core

#include Projection

layout(binding = 0) buffer pixel
{
	PixelData data[];
} pixelDataBuffer;

layout(binding = 1) buffer surface
{
	int data[];
} surfaceBuffer;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main()
{
	uint indexX = (gl_WorkGroupID.x * gl_WorkGroupSize.x) + gl_LocalInvocationID.x;
	uint indexY = (gl_WorkGroupID.y * gl_WorkGroupSize.y) + gl_LocalInvocationID.y;

	uint index = indexX + (indexY * screenWidth);

	vec3 colour = (((pixelDataBuffer.data[index].PixelNormal.xyz * 0.5f) + vec3(0.5f)) * 255) * pixelDataBuffer.data[index].bPainted;

	surfaceBuffer.data[index] = int((0 << 24) | (uint(colour.r) << 16) | (uint(colour.g) << 8) | (uint(colour.b)));
}
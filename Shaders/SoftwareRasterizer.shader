#version 460 core

#include Projection

layout(binding = 0) buffer triangle
{
	TriangleData data[];
} triangleBuffer;

layout(binding = 1) buffer pixel
{
	uint data[];
} pixelDataBuffer;

layout(binding = 2) buffer depth
{
	float data[];
} depthBuffer;


layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main()
{
	uint indexX = (gl_WorkGroupID.x * gl_WorkGroupSize.x) + gl_LocalInvocationID.x;
	uint indexY = (gl_WorkGroupID.y * gl_WorkGroupSize.y) + gl_LocalInvocationID.y;

	vec3 samp = vec3(indexX, indexY, 1.0f);

	uint index = indexX + (indexY * screenWidth);

	depthBuffer.data[index] = 1000000.0f;

	uint indicesLen = triangleBuffer.data.length();

	pixelDataBuffer.data[index] = 0;

	for (uint j = 0; j < indicesLen; ++j)
	{
		vec3 E0 = triangleBuffer.data[j].vertexData[0].pos.xyz;
		vec3 E1 = triangleBuffer.data[j].vertexData[1].pos.xyz;
		vec3 E2 = triangleBuffer.data[j].vertexData[2].pos.xyz;

		if (PixelInsideTriangle(E0, E1, E2, samp))
		{
			float depth = 0.0f;
			if (depth < depthBuffer.data[index])
			{
				vec3 colour = ((triangleBuffer.data[j].vertexData[0].norm.xyz * 0.5f) + vec3(0.5f)) * 255;
				pixelDataBuffer.data[index] = uint((0 << 24) | (uint(colour.r) << 16) | (uint(colour.g) << 8) | (uint(colour.b))); //vec4(vertexBuffer.data[indexBuffer.data[indicesIndex]].norm * vec3(0.5f) + vec3(0.5f), 1.0f) * 255;
				depthBuffer.data[index] = depth;
			}
		}
	}
}
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

	depthBuffer.data[index] = 100000000000000.0f;

	uint indicesLen = triangleBuffer.data.length();

	pixelDataBuffer.data[index] = 0;

	for (uint j = 0; j < indicesLen; ++j)
	{
		vec4 v0Raster = triangleBuffer.data[j].vertexData[0].pos;
		vec4 v1Raster = triangleBuffer.data[j].vertexData[1].pos;
		vec4 v2Raster = triangleBuffer.data[j].vertexData[2].pos;

		mat3 model =
		{
			{ v0Raster.x, v1Raster.x, v2Raster.x },
			{ v0Raster.y, v1Raster.y, v2Raster.y },
			{ v0Raster.w, v1Raster.w, v2Raster.w },
		};

		vec3 depthConst = model * vec3(1.0f);
		float depth = (depthConst.x * samp.x) + (depthConst.y * samp.y) + depthConst.z;

		model *= identity;
		model = inverse(model);

		vec3 E0 = model[0] / (abs(model[0].x) + abs(model[0].y));
		vec3 E1 = model[1] / (abs(model[1].x) + abs(model[1].y));
		vec3 E2 = model[2] / (abs(model[2].x) + abs(model[2].y));

		if (depth < depthBuffer.data[index])
		{
			if (PixelInsideTriangle(E0, E1, E2, samp))
			{
				vec3 colour = ((triangleBuffer.data[j].vertexData[0].norm.xyz * 0.5f) + vec3(0.5f)) * 255;
				pixelDataBuffer.data[index] = uint((0 << 24) | (uint(colour.r) << 16) | (uint(colour.g) << 8) | (uint(colour.b)));
				depthBuffer.data[index] = depth;
			}
		}
	}
}
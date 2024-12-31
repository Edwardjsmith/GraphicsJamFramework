#version 460 core

#include Projection

layout(binding = 0) buffer triangle
{
	TriangleData data[];
} triangleBuffer;

layout(binding = 1) buffer vertex
{
	VertexData data[];
} vertexBuffer;

layout(binding = 2) buffer pixel
{
	PixelData data[];
} pixelDataBuffer;

layout(binding = 3) buffer depth
{
	float data[];
} depthBuffer;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main()
{
	//From NDC to clip
	vec4 v0Clip = ProjectionViewModel * vec4(vertexBuffer.data[triangleBuffer.data[gl_GlobalInvocationID.x].TriangleVertIndex[0]].pos.xyz, 1.0f);
	vec4 v1Clip = ProjectionViewModel * vec4(vertexBuffer.data[triangleBuffer.data[gl_GlobalInvocationID.x].TriangleVertIndex[1]].pos.xyz, 1.0f);
	vec4 v2Clip = ProjectionViewModel * vec4(vertexBuffer.data[triangleBuffer.data[gl_GlobalInvocationID.x].TriangleVertIndex[2]].pos.xyz, 1.0f);

	vec4 v0Raster = ToRaster(v0Clip);
	vec4 v1Raster = ToRaster(v1Clip);
	vec4 v2Raster = ToRaster(v2Clip);

	mat3 model =
	{
		{ v0Raster.x, v1Raster.x, v2Raster.x },
		{ v0Raster.y, v1Raster.y, v2Raster.y },
		{ v0Raster.w, v1Raster.w, v2Raster.w },
	};

	model *= identity;
	model = inverse(model);

	vec3 E0 = model[0] / (abs(model[0].x) + abs(model[0].y));
	vec3 E1 = model[1] / (abs(model[1].x) + abs(model[1].y));
	vec3 E2 = model[2] / (abs(model[2].x) + abs(model[2].y));

	float wmin = min(min(-v0Raster.w, -v1Raster.w), -v2Raster.w);
	float wmax = max(max(v0Raster.w, v1Raster.w), v2Raster.w);

	uint ymin = uint(clamp(min(min(v0Raster.y, v1Raster.y), v2Raster.y) * wmin, 0, screenHeight));
	uint ymax = uint(clamp(max(max(v0Raster.y, v1Raster.y), v2Raster.y) * wmax, 0, screenHeight));
	uint xmin = uint(clamp(min(min(v0Raster.x, v1Raster.x), v2Raster.x) * wmin, 0, screenWidth));
	uint xmax = uint(clamp(max(max(v0Raster.x, v1Raster.x), v2Raster.x) * wmax, 0, screenWidth));

	for (uint indexX = xmin; indexX < xmax; ++indexX)
	{
		for (uint indexY = ymin; indexY < ymax; ++indexY)
		{
			vec3 samp = vec3(indexX, indexY, 1.0f);

			if (PixelInsideTriangle(E0, E1, E2, samp))
			{
				uint index = indexX + (indexY * screenWidth);
				depthBuffer.data[index] = 100000000000000.0f;

				vec3 depthConst = model * vec3(1.0f);
				float depth = (depthConst.x * samp.x) + (depthConst.y * samp.y) + depthConst.z;
				
				if (depth < depthBuffer.data[index])
				{
					depthBuffer.data[index] = depth;
					pixelDataBuffer.data[index].PixelPos = vec4(samp.xy, depthBuffer.data[index], 1.0f);
					pixelDataBuffer.data[index].PixelNormal = (vertexBuffer.data[triangleBuffer.data[gl_GlobalInvocationID.x].TriangleVertIndex[0]].norm + vertexBuffer.data[triangleBuffer.data[gl_GlobalInvocationID.x].TriangleVertIndex[1]].norm + vertexBuffer.data[triangleBuffer.data[gl_GlobalInvocationID.x].TriangleVertIndex[2]].norm) / 3;
					pixelDataBuffer.data[index].bPainted = 1;
				}
			}
		}
	}
}
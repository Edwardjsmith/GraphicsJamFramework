#version 460 core

#include Projection

layout(binding = 0) buffer vertex
{
	VertexData data[];
} vertexBuffer;

layout(binding = 1) buffer index
{
	int data[];
} indexBuffer;

layout(binding = 2) buffer triangle
{
	TriangleData data[];
} triangleBuffer;

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;
void main()
{
	uint Index = ((gl_WorkGroupID.x * gl_WorkGroupSize.x) + gl_LocalInvocationID.x) * 3;

	//From NDC to clip
	vec4 v0Clip = ProjectionViewModel * vertexBuffer.data[indexBuffer.data[Index]].pos;
	vec4 v1Clip = ProjectionViewModel * vertexBuffer.data[indexBuffer.data[Index + 1]].pos;
	vec4 v2Clip = ProjectionViewModel * vertexBuffer.data[indexBuffer.data[Index + 2]].pos;

	float ymin = min(min(v0Clip.y, v1Clip.y), v2Clip.y);
	float xmax = max(max(v0Clip.x, v1Clip.x), v2Clip.x);
	float ymax = max(max(v0Clip.y, v1Clip.y), v2Clip.y);
	float xmin = min(min(v0Clip.x, v1Clip.x), v2Clip.x);

	if (((xmin > v0Clip.w)) || ((xmax < -v0Clip.w)) || ((ymin > v0Clip.w) || (ymax < -v0Clip.w)) == false)
	{

		//Clip to raster
		vec4 v0Raster = ToRaster(v0Clip);
		vec4 v1Raster = ToRaster(v1Clip);
		vec4 v2Raster = ToRaster(v2Clip);

		mat3 model =
		{
			{ v0Raster.x, v1Raster.x, v2Raster.x },
			{ v0Raster.y, v1Raster.y, v2Raster.y },
			{ v0Raster.w, v1Raster.w, v2Raster.w },
		};

		triangleBuffer.data[Index].bInitialized = 0;

		//If <= 0, back facing triangle so continue
		if (Determinant3X3(model) >= 0.0f)
		{
			triangleBuffer.data[Index].vertexData[0].pos = v0Raster;
			triangleBuffer.data[Index].vertexData[0].norm = vertexBuffer.data[indexBuffer.data[Index]].norm;

			triangleBuffer.data[Index].vertexData[1].pos = v1Raster;
			triangleBuffer.data[Index].vertexData[1].norm = vertexBuffer.data[indexBuffer.data[Index + 1]].norm;

			triangleBuffer.data[Index].vertexData[2].pos = v2Raster;
			triangleBuffer.data[Index].vertexData[2].norm = vertexBuffer.data[indexBuffer.data[Index + 2]].norm;

			triangleBuffer.data[Index].bInitialized = 1;
		}
	}
}
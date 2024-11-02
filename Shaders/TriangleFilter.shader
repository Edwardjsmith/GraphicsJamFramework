#version 460 core

#include Projection

layout(binding = 0) buffer depth
{
	int data[];
} depthBuffer;

layout(binding = 1) buffer vertex
{
	VertexData data[];
} vertexBuffer;

layout(binding = 2) buffer index
{
	int data[];
} indexBuffer;

layout(binding = 3) buffer visibleTriangle
{
	TriangleData data[];
} visibleTriangleBuffer;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main()
{
	uint Index = gl_GlobalInvocationID.x * 3;

	//From NDC to clip
	vec4 v0Clip = ProjectionViewModel * vec4(vertexBuffer.data[indexBuffer.data[Index]].pos, 1.0f);
	vec4 v1Clip = ProjectionViewModel * vec4(vertexBuffer.data[indexBuffer.data[Index + 1]].pos, 1.0f);
	vec4 v2Clip = ProjectionViewModel * vec4(vertexBuffer.data[indexBuffer.data[Index + 2]].pos, 1.0f);

	//float ymin = min(min(v0Clip.y, v1Clip.y), v2Clip.y);
	//float xmax = max(max(v0Clip.x, v1Clip.x), v2Clip.x);
	//float ymax = max(max(v0Clip.y, v1Clip.y), v2Clip.y);
	//float xmin = min(min(v0Clip.x, v1Clip.x), v2Clip.x);

	//if ((xmin > 1.0f) || (ymin > 1.0f))
	//{
	//	continue;
	//}

	visibleTriangleBuffer.data[Index].bInitialized = 0;
	
	//If <= 0, back facing triangle so continue
	if (Determinant3X3(mat3(v0Clip.xyw, v1Clip.xyw, v2Clip.xyw)) >= 0.0f)
	{
		//visibleTriangleBuffer.data[Index].vertexData[0].pos = v0Clip.xyz;
		//visibleTriangleBuffer.data[Index].vertexData[0].norm = vertexBuffer.data[indexBuffer.data[Index]].norm;

		//visibleTriangleBuffer.data[Index].vertexData[1].pos = v1Clip.xyz;
		//visibleTriangleBuffer.data[Index].vertexData[1].norm = vertexBuffer.data[indexBuffer.data[Index + 1]].norm;

		//visibleTriangleBuffer.data[Index].vertexData[2].pos = v2Clip.xyz;
		//visibleTriangleBuffer.data[Index].vertexData[2].norm = vertexBuffer.data[indexBuffer.data[Index + 2]].norm;

		visibleTriangleBuffer.data[Index].bInitialized = 1;
	}
}
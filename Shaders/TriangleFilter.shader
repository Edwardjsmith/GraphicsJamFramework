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

layout(binding = 3) buffer visibleIndex
{
	int data[];
} visibleIndexBuffer;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main()
{
	uint indicesIndex = gl_GlobalInvocationID.x * 3;

	//From NDC to clip
	vec4 v0Clip = ProjectionViewModel * vec4(vertexBuffer.data[indexBuffer.data[indicesIndex]].pos, 1.0f);
	vec4 v1Clip = ProjectionViewModel * vec4(vertexBuffer.data[indexBuffer.data[indicesIndex + 1]].pos, 1.0f);
	vec4 v2Clip = ProjectionViewModel * vec4(vertexBuffer.data[indexBuffer.data[indicesIndex + 2]].pos, 1.0f);

	//float ymin = min(min(v0Clip.y, v1Clip.y), v2Clip.y);
	//float xmax = max(max(v0Clip.x, v1Clip.x), v2Clip.x);
	//float ymax = max(max(v0Clip.y, v1Clip.y), v2Clip.y);
	//float xmin = min(min(v0Clip.x, v1Clip.x), v2Clip.x);

	//if ((xmin > 1.0f) || (ymin > 1.0f))
	//{
	//	continue;
	//}

	visibleIndexBuffer.data[indicesIndex] = -1;
	visibleIndexBuffer.data[indicesIndex + 1] = -1;
	visibleIndexBuffer.data[indicesIndex + 2] = -1;
	
	//If <= 0, back facing triangle so continue
	if (Determinant3X3(mat3(v0Clip.xyw, v1Clip.xyw, v2Clip.xyw)) >= 0.0f)
	{
		visibleIndexBuffer.data[indicesIndex] = int(indexBuffer.data[indicesIndex]);
	}
}
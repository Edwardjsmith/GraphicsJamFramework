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

layout(binding = 2) buffer index
{
	int data[];
} indexBuffer;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main()
{
	uint indicesIndex = gl_GlobalInvocationID.x * 3;

	//From NDC to clip
	vec4 v0Clip = ProjectionViewModel * vertexBuffer.data[indexBuffer.data[indicesIndex]].pos;
	vec4 v1Clip = ProjectionViewModel * vertexBuffer.data[indexBuffer.data[indicesIndex + 1]].pos;
	vec4 v2Clip = ProjectionViewModel * vertexBuffer.data[indexBuffer.data[indicesIndex + 2]].pos;

	//float ymin = min(min(v0Clip.y, v1Clip.y), v2Clip.y);
	//float xmax = max(max(v0Clip.x, v1Clip.x), v2Clip.x);
	//float ymax = max(max(v0Clip.y, v1Clip.y), v2Clip.y);
	//float xmin = min(min(v0Clip.x, v1Clip.x), v2Clip.x);

	//if ((xmin > 1.0f) || (ymin > 1.0f))
	//{
	//	continue;
	//}

	//If <= 0, back facing triangle so continue
	if (Determinant3X3(mat3(v0Clip.xyw, v1Clip.xyw, v2Clip.xyw)) >= 0.0f)
	{
		triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[0].pos = v0Clip;
		triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[0].norm = vertexBuffer.data[indexBuffer.data[indicesIndex]].norm;

		triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[1].pos = v1Clip;
		triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[1].norm = vertexBuffer.data[indexBuffer.data[indicesIndex + 1]].norm;

		triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[2].pos = v2Clip;
		triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[2].norm = vertexBuffer.data[indexBuffer.data[indicesIndex + 2]].norm;
	}
}
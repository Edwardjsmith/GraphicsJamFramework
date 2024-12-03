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
	vec4 v0Clip = ProjectionViewModel * vec4(vertexBuffer.data[indexBuffer.data[indicesIndex]].pos.xyz, 1.0f);
	vec4 v1Clip = ProjectionViewModel * vec4(vertexBuffer.data[indexBuffer.data[indicesIndex + 1]].pos.xyz, 1.0f);
	vec4 v2Clip = ProjectionViewModel * vec4(vertexBuffer.data[indexBuffer.data[indicesIndex + 2]].pos.xyz, 1.0f);

	//float ymin = min(min(v0Clip.y, v1Clip.y), v2Clip.y);
	//float xmax = max(max(v0Clip.x, v1Clip.x), v2Clip.x);
	//float ymax = max(max(v0Clip.y, v1Clip.y), v2Clip.y);
	//float xmin = min(min(v0Clip.x, v1Clip.x), v2Clip.x);

	vec4 v0Raster = ToRaster(v0Clip);
	vec4 v1Raster = ToRaster(v1Clip);
	vec4 v2Raster = ToRaster(v2Clip);

	mat3 model =
	{
		{ v0Raster.x, v1Raster.x, v2Raster.x },
		{ v0Raster.y, v1Raster.y, v2Raster.y },
		{ v0Raster.w, v1Raster.w, v2Raster.w },
	};

	//vec3 depthConst = model * vec3(1.0f);==
	//float depth = (depthConst.x * samp.x) + (depthConst.y * samp.y) + depthConst.z;
	model *= identity;
	model = inverse(model);

	vec3 E0 = model[0] / (abs(model[0].x) + abs(model[0].y));
	vec3 E1 = model[1] / (abs(model[1].x) + abs(model[1].y));
	vec3 E2 = model[2] / (abs(model[2].x) + abs(model[2].y));

	triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[0].pos = vec4(E0, 1.0f);
	triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[0].norm = vertexBuffer.data[indexBuffer.data[indicesIndex]].norm;

	triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[1].pos = vec4(E1, 1.0f);
	triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[1].norm = vertexBuffer.data[indexBuffer.data[indicesIndex + 1]].norm;

	triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[2].pos = vec4(E2, 1.0f);
	triangleBuffer.data[gl_GlobalInvocationID.x].vertexData[2].norm = vertexBuffer.data[indexBuffer.data[indicesIndex + 2]].norm;

	triangleBuffer.data[gl_GlobalInvocationID.x].bInitialized = int(Determinant3X3(model) < 0.0f);
}

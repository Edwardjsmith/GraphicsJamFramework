#version 460 core

#include Projection

struct PixelData
{
	vec4 color;
};

struct VertexData
{
	vec3 pos;
	vec3 norm;
};

layout(binding = 0) buffer pixel
{
	PixelData data[];
} pixelDataBuffer;

layout(binding = 1) buffer depth
{
	float data[];
} depthBuffer;

layout(binding = 2) buffer vertex
{
	VertexData data[];
} vertexBuffer;

layout(binding = 3) buffer index
{
	int data[];
} indexBuffer;

uniform mat4 Transform;

vec4 black = vec4(0);

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main()
{
	uint indexX = (gl_WorkGroupID.x * gl_WorkGroupSize.x) + gl_LocalInvocationID.x;
	uint indexY = (gl_WorkGroupID.y * gl_WorkGroupSize.y) + gl_LocalInvocationID.y;

	vec3 samp = vec3(indexX, indexY, 1.0f);

	uint index = indexX + (indexY * screenWidth);
	depthBuffer.data[index] = 100000000000000.0f;
	pixelDataBuffer.data[index].color = black;
	
	uint indicesLen = indexBuffer.data.length() / 3;

	for (uint j = 0; j < indicesLen; ++j)
	{
		uint indicesIndex = j * 3;

		//From NDC to clip
		vec4 v0Clip = ProjectionView * Transform * vec4(vertexBuffer.data[indexBuffer.data[indicesIndex]].pos, 1.0f);
		vec4 v1Clip = ProjectionView * Transform * vec4(vertexBuffer.data[indexBuffer.data[indicesIndex + 1]].pos, 1.0f);
		vec4 v2Clip = ProjectionView * Transform * vec4(vertexBuffer.data[indexBuffer.data[indicesIndex + 2]].pos, 1.0f);

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

		//If >= 0, back facing triangle so continue
		if (Determinant3X3(model) >= 0.0f)
		{
			continue;
		}

		vec3 depthConst = model * vec3(1.0f);
		float depth = (depthConst.x * samp.x) + (depthConst.y * samp.y) + depthConst.z;

		if (depth >= depthBuffer.data[index])
		{
			continue;
		}

		model *= identity;
		model = inverse(model);

		vec3 E0 = model[0] / (abs(model[0].x) + abs(model[0].y));
		vec3 E1 = model[1] / (abs(model[1].x) + abs(model[1].y));
		vec3 E2 = model[2] / (abs(model[2].x) + abs(model[2].y));

		bool alpha = EvaluateEdgeFunction(E0, samp.xy);
		bool beta = EvaluateEdgeFunction(E1, samp.xy);
		bool gamma = EvaluateEdgeFunction(E2, samp.xy);

		if (alpha && beta && gamma)
		{
			pixelDataBuffer.data[index].color = vec4(vertexBuffer.data[indexBuffer.data[indicesIndex]].norm * vec3(0.5f) + vec3(0.5f), 1.0f) * 255;
			depthBuffer.data[index] = depth;
		}
	}

	if(pixelDataBuffer.data[index].color == black)
	{
		float colourX = (indexX / (screenWidth * 2.0f)) * 255.0f;
		float colourY = (indexY / (screenHeight * 2.0f)) * 255.0f;

		pixelDataBuffer.data[index].color = vec4(vec3(colourX, colourY, 0), 1.0f);
	}
}
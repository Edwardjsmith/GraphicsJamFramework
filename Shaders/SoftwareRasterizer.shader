#version 460 core

#include Projection

struct PixelData
{
	vec4 color;
};

layout(binding = 0) buffer pixel
{
	PixelData data[];
} pixelDataBuffer;

layout(binding = 1) buffer depth
{
	float data[];
} depthBuffer;

uniform mat4 objectTransform[3];

//layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
//void main()
//{
//	uint indexX = (gl_WorkGroupID.x * gl_WorkGroupSize.x) + gl_LocalInvocationID.x;
//	uint indexY = (gl_WorkGroupID.y * gl_WorkGroupSize.y) + gl_LocalInvocationID.y;
//
//	uint index = indexX + (indexY * screenWidth);
//	depthBuffer.data[index] = 10000000.0f;
//	pixelDataBuffer.data[index].color = vec4(0);
//
//	for (uint i = 0; i < 1; ++i)//objectTransform.length(); ++i)
//	{
//		//for (uint j = 0; j < indices.length(); ++j)
//		{
//			//Local space coords
//			uint indicesIndex = 0;//j * 3;
//			vec4 left = vec4(vertices[indices[indicesIndex]] * 0.5f, 1.0f);
//			vec4 right = vec4(vertices[indices[indicesIndex + 1]] * 0.5f, 1.0f);
//			vec4 top = vec4(vertices[indices[indicesIndex + 2]] * 0.5f, 1.0f);
//
//			Triangle triangle;
//			BuildTriangle(triangle, left.xyz, right.xyz, top.xyz);
//			ProjectTriangle(triangle, objectTransform[i], left, right, top);
//
//			left = ToRaster(left);
//			right = ToRaster(right);
//			top = ToRaster(top);
//
//			vec3 pixelCoords = vec3(indexX, indexY, 1.0f);
//
//			float leftEdge = edgeFunc(left.xyz, right.xyz, pixelCoords);
//			float rightEdge = edgeFunc(right.xyz, top.xyz, pixelCoords);
//			float topEdge = edgeFunc(top.xyz, left.xyz, pixelCoords);
//
//			if (leftEdge >= 0.0f && rightEdge >= 0.0f && topEdge >= 0.0f)
//			{
//				pixelDataBuffer.data[index].color = vec4(0.0f, 127.5f * i, 255.0f, 1.0f);
//
//
//			}
//		}
//	}
//
//	if (pixelDataBuffer.data[index].color.z == 0.0f)
//	{
//		float colourX = (indexX / (screenWidth * 2.0f)) * 255.0f;
//		float colourY = (indexY / (screenHeight * 2.0f)) * 255.0f;
//
//		pixelDataBuffer.data[index].color = vec4(vec3(colourX, colourY, 0), 1.0f);
//	}
//}

//layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
//void main()
//{
//	uint indexX = (gl_WorkGroupID.x * gl_WorkGroupSize.x) + gl_LocalInvocationID.x;
//	uint indexY = (gl_WorkGroupID.y * gl_WorkGroupSize.y) + gl_LocalInvocationID.y;
//
//	vec3 samp = vec3(indexX, indexY, 1.0f);
//
//	uint index = indexX + (indexY * screenWidth);
//	depthBuffer.data[index] = 10000000.0f;
//	pixelDataBuffer.data[index].color = vec4(0);
//
//	//Assign verts in NDC space
//	vec3 v0 = vertices2D[0];
//	vec3 v1 = vertices2D[1];
//	vec3 v2 = vertices2D[2];
//
//	//Convert verts to raster
//	v0 = ToRaster2D(v0);
//	v1 = ToRaster2D(v1);
//	v2 = ToRaster2D(v2);
//
//
//	mat3 model =
//	{
//		{ v0.x, v1.x, v2.x },
//		{ v0.y, v1.y, v2.y },
//		{ v0.z, v1.z, v2.z },
//	};
//
//	if (Determinant3X3(model) != 0.0f)
//	{
//		model = inverse(model);
//		model *= identity;
//
//		v0 = model[0];
//		v1 = model[1];
//		v2 = model[2];
//
//		float alpha = dot(v0, samp);
//		float beta = dot(v1, samp);
//		float gamma = dot(v2, samp);
//
//		if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f)
//		{
//			pixelDataBuffer.data[index].color = vec4(0.0f, 0.0f, 255.0f, 1.0f);
//		}
//		else
//		{
//			float colourX = (indexX / (screenWidth * 2.0f)) * 255.0f;
//			float colourY = (indexY / (screenHeight * 2.0f)) * 255.0f;
//
//			pixelDataBuffer.data[index].color = vec4(vec3(colourX, colourY, 0), 1.0f);
//		}
//	}
//}

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main()
{
	uint indexX = (gl_WorkGroupID.x * gl_WorkGroupSize.x) + gl_LocalInvocationID.x;
	uint indexY = (gl_WorkGroupID.y * gl_WorkGroupSize.y) + gl_LocalInvocationID.y;

	vec3 samp = vec3(indexX, indexY, 1.0f);

	uint index = indexX + (indexY * screenWidth);
	depthBuffer.data[index] = 0.0f;

	vec4 black = vec4(0);
	pixelDataBuffer.data[index].color = black;

	for (uint i = 0; i < objectTransform.length(); ++i)
	{
		for (uint j = 0; j < indices.length() / 3; ++j)
		{
			uint indicesIndex = j * 3;

			//From NDC to clip
			vec4 v0Clip = ProjectionView * objectTransform[i] * vec4(vertices[indices[indicesIndex]], 1.0f);
			vec4 v1Clip = ProjectionView * objectTransform[i] * vec4(vertices[indices[indicesIndex + 1]], 1.0f);
			vec4 v2Clip = ProjectionView * objectTransform[i] * vec4(vertices[indices[indicesIndex + 2]], 1.0f);

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

			model = inverse(model);
			vec3 depthConst = model * vec3(1);

			model *= identity;

			float alpha = dot(model[0], samp);
			float beta = dot(model[1], samp);
			float gamma = dot(model[2], samp);

			if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f)
			{
				float oneOverW = (depthConst.x * samp.x) + (depthConst.y * samp.y) + depthConst.z;

				if (oneOverW >= depthBuffer.data[index])
				{
					pixelDataBuffer.data[index].color = vec4(0.0f, 0.0f, 255.0f, 1.0f);
					depthBuffer.data[index] = oneOverW;
				}
			}
		}
	}

	if(pixelDataBuffer.data[index].color == black)
	{
		float colourX = (indexX / (screenWidth * 2.0f)) * 255.0f;
		float colourY = (indexY / (screenHeight * 2.0f)) * 255.0f;

		pixelDataBuffer.data[index].color = vec4(vec3(colourX, colourY, 0), 1.0f);
	}
}
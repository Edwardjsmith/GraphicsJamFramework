#version 460 core

struct PixelData
{
	vec4 color;
};

layout(binding = 0) buffer block
{
	PixelData data[];
} outBuffer;

// This defines the dimensions of the compute shaders local group.
// This is the smallest group of times that this compute shader will be invoked on the data set.
// Multiplying all 3 values gives us the "volume" of one operation.
// When glDispatchCompute is called, it takes in 3 values that define how many local groups to run.
// In this case, we just want to operate on one thing at a time.
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(location = 0) uniform float delta;

int maxIndex = 640 * 480;

void main()
{
	for (int i = 0; i < maxIndex; ++i)
	{
		outBuffer.data[i].color += vec4(0, 0, 255, 0) * delta;
	}
}
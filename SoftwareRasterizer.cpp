#include "SoftwareRasterizer.h"

struct DepthBuffer
{
	float depth = 100000.0f;
};

void SoftwareRasterizer::Dispatch(void* OutData, int dispatchCount)
{
	std::vector<DepthBuffer> depthBuffer(SCREEN_WIDTH * SCREEN_HEIGHT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_TriangleRasterBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_ShaderParams.TriangleDataSize, (GLvoid*)m_ShaderParams.TriangleData, GL_DYNAMIC_READ);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_VertexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_ShaderParams.VertexDataSize, (GLvoid*)m_ShaderParams.VertexData, GL_DYNAMIC_READ);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_PixelBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_ShaderParams.PixelsSize, (GLvoid*)OutData, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_DepthBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DepthBuffer) * depthBuffer.size(), (GLvoid*)depthBuffer.data(), GL_DYNAMIC_DRAW);

	glDispatchCompute(dispatchCount, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_PixelBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_ShaderParams.PixelsSize, (GLvoid*)OutData);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SoftwareRasterizer::GenerateBuffers()
{
	glGenBuffers(1, &m_TriangleRasterBuffer);
	glGenBuffers(1, &m_VertexBuffer);
	glGenBuffers(1, &m_PixelBuffer);
	glGenBuffers(1, &m_DepthBuffer);
}

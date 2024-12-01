#include "SoftwareRasterizer.h"

void SoftwareRasterizer::Dispatch(void* OutData, int dispatchCount)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_TriangleRasterBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_ShaderParams.TriangleDataSize, (GLvoid*)m_ShaderParams.TriangleData);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_DepthBuffer);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_PixelBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_ShaderParams.PixelsSize, 0, GL_DYNAMIC_DRAW);

	glDispatchCompute(SCREEN_WIDTH / 32, SCREEN_HEIGHT / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_PixelBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_ShaderParams.PixelsSize, (GLvoid*)OutData);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SoftwareRasterizer::GenerateBuffers()
{
	glGenBuffers(1, &m_TriangleRasterBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TriangleRasterBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TriangleData) * 512, 0, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_PixelBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_PixelBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, 0, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_DepthBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DepthBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * (SCREEN_WIDTH * SCREEN_HEIGHT), 0, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

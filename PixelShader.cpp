#include "PixelShader.h"

void PixelShader::Dispatch(void* OutData, int dispatchCount)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_PixelBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_ShaderParams.PixelDataSize, (GLvoid*)m_ShaderParams.PixelData, GL_DYNAMIC_READ);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_SurfaceBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_ShaderParams.SurfaceSize, (GLvoid*)OutData, GL_DYNAMIC_DRAW);

	glDispatchCompute(SCREEN_WIDTH / 16, SCREEN_HEIGHT / 16, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_SurfaceBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_ShaderParams.SurfaceSize, (GLvoid*)OutData);
}

void PixelShader::GenerateBuffers()
{
	glGenBuffers(1, &m_PixelBuffer);
	glGenBuffers(1, &m_SurfaceBuffer);
}

#include "TriangleFilter.h"

void TriangleFilter::Dispatch(void* OutData, int dispatchCount)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_VertexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_ShaderParams.VertexDataSize, (GLvoid*)m_ShaderParams.VertexData, GL_DYNAMIC_READ);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_IndexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_ShaderParams.IndexDataSize, (GLvoid*)m_ShaderParams.IndexData, GL_DYNAMIC_READ);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_TriangleFilterBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_ShaderParams.TriangleDataSize, 0, GL_DYNAMIC_DRAW);

	glDispatchCompute(dispatchCount, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_ShaderParams.TriangleDataSize, (GLvoid*)OutData);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TriangleFilter::GenerateBuffers()
{
	glGenBuffers(1, &m_TriangleFilterBuffer);
	glGenBuffers(1, &m_VertexBuffer);
	glGenBuffers(1, &m_IndexBuffer);
}
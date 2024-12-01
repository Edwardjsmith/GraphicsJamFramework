#pragma once
#include "ComputeShader.h"

struct TriangleFilterParameters
{
	void* VertexData = nullptr;
	size_t VertexDataSize = 0;

	void* IndexData = nullptr;
	size_t IndexDataSize = 0;

	size_t TriangleDataSize = 0;
};

struct VertexInput
{
	glm::vec3   Pos;
	glm::vec3   Normal;
	glm::vec2	UV;
};

class TriangleFilter : public ComputeShader
{
public:

	void Dispatch(void* OutData, int dispatchCount = 8) override;
	void GenerateBuffers() override;

	void SetTriangleFilterParameters(const TriangleFilterParameters& parameters) { m_ShaderParams = parameters; }

private:

	GLuint m_TriangleFilterBuffer = 0;
	GLuint m_VertexBuffer = 0;
	GLuint m_IndexBuffer = 0;

	TriangleFilterParameters m_ShaderParams;
};


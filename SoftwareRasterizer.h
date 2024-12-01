#pragma once
#include "ComputeShader.h"

struct SoftwareRasterizerParameters
{
	void* TriangleData = nullptr;
	size_t TriangleDataSize = 0;

	size_t PixelsSize = 0;
};

struct PixelData
{
	PixelData()
	{
		color = glm::vec4(0);
	}

	glm::vec4 color;
};

class SoftwareRasterizer : public ComputeShader
{
public:

	void Dispatch(void* OutData, int dispatchCount = 8) override;
	void GenerateBuffers() override;
	void SetSoftwareRasterizerParameters(const SoftwareRasterizerParameters& parameters) { m_ShaderParams = parameters; }

private:

	GLuint m_TriangleRasterBuffer = 0;
	GLuint m_PixelBuffer = 0;
	GLuint m_DepthBuffer = 0;

	SoftwareRasterizerParameters m_ShaderParams;
};


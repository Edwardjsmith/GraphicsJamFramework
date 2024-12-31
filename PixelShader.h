#pragma once
#include "ComputeShader.h"

struct PixelShaderParameters
{
    void* PixelData = nullptr;
    size_t PixelDataSize;

    size_t SurfaceSize;
};

class PixelShader : public ComputeShader
{
public:

    void Dispatch(void* OutData, int dispatchCount = 8) override;
    void GenerateBuffers() override;
    void SetPixelShaderParameters(const PixelShaderParameters& params) { m_ShaderParams = params; }

private:

    PixelShaderParameters m_ShaderParams;

    GLuint m_PixelBuffer = 0;
    GLuint m_SurfaceBuffer = 0;
};


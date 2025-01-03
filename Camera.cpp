﻿#include "Camera.h"
#include "PixelShader.h"

Camera::Camera(glm::vec3& position, glm::vec3& targetLoc, glm::vec3& worldUp, float speed, float pitch, float yaw)
{
	m_position = position;
	m_direction = glm::normalize(targetLoc - m_position);

	m_right = glm::normalize(glm::cross(worldUp, m_direction));
	m_up = glm::normalize(glm::cross(m_direction, m_right));
	m_forward = glm::normalize(m_direction);

	m_speed = speed;

	m_pitch = pitch;
	m_yaw = yaw;

	m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
}

Camera::~Camera()
{
	for (int i = 0; i < 2; ++i)
	{
		m_ComputeShaders[i].reset();
	}
}

void Camera::Update(float delta)
{
	if (m_pitch > 89.0f)
	{
		m_pitch = 89.0f;
	}
	if (m_pitch < -89.0f)
	{
		m_pitch = -89.0f;
	}

	m_direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_direction.y = sin(glm::radians(m_pitch));

	m_right = glm::normalize(glm::cross(GWorldUp, m_direction));
	m_up = glm::normalize(glm::cross(m_direction, m_right));

	m_forward = glm::normalize(m_direction);

	m_projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCREEN_WIDTH / SCREEN_HEIGHT), 10.0f, 100.0f);
	m_view = glm::lookAt(m_position, m_position + m_forward, m_up);

	m_objTransform = glm::rotate(m_objTransform, delta, glm::vec3(0.0f, 1.0f, 0.0f));
}

#if RAYTRACER 1
const int GDispatchX = SCREEN_WIDTH / 16;
const int GDispatchY = SCREEN_HEIGHT / 16;
#else

int GDispatchX = 0;
int GDispatchY = 0;

std::vector<VertexInput> GVertexData;
std::vector<uint32_t> GIndexData;
#endif


void Camera::Draw(SDL_Surface* const surface)
{
	//SDL_FillRect(surface, NULL, 0x000000);

	auto triangleData = std::vector<TriangleData>((GIndexData.size() / 3));
	size_t triangleSize = (sizeof(TriangleData) * (triangleData.size()));

	TriangleFilter* const triangleFilter = dynamic_cast<TriangleFilter*>(m_ComputeShaders[0].get());

	if (triangleFilter)
	{
		triangleFilter->Use();
		triangleFilter->SetTriangleFilterParameters(TriangleFilterParameters{ GVertexData.data(), (sizeof(VertexInput) * GVertexData.size()), GIndexData.data(), (sizeof(int32_t) * GIndexData.size()), triangleSize });
		triangleFilter->SetInt("screenWidth", SCREEN_WIDTH);
		triangleFilter->SetInt("screenHeight", SCREEN_HEIGHT);
		triangleFilter->SetMatrix("ProjectionViewModel", m_projection * m_view * m_objTransform);
		triangleFilter->Dispatch(triangleData.data(), triangleData.size());
	}

	auto pixelData = std::vector<PixelData>(SCREEN_WIDTH * SCREEN_HEIGHT);
	size_t pixelSize = (sizeof(PixelData) * pixelData.size());

	SoftwareRasterizer* const rasterizer = dynamic_cast<SoftwareRasterizer*>(m_ComputeShaders[1].get());

	if (rasterizer)
	{
		triangleData.erase(std::remove_if(triangleData.begin(), triangleData.end(),
			[](TriangleData& tri) { return  tri.TriangleVertIndex[3] == 0; }), triangleData.end());

		triangleSize = (sizeof(TriangleData) * (triangleData.size()));

		rasterizer->Use();
		rasterizer->SetSoftwareRasterizerParameters(SoftwareRasterizerParameters{ triangleData.data(), triangleSize,  GVertexData.data(), (sizeof(VertexInput) * GVertexData.size()), pixelSize});
		rasterizer->SetInt("screenWidth", SCREEN_WIDTH);
		rasterizer->SetInt("screenHeight", SCREEN_HEIGHT);
		rasterizer->SetMatrix("ProjectionViewModel", m_projection * m_view * m_objTransform);
		
		rasterizer->Dispatch(pixelData.data(), triangleData.size());
	}

	PixelShader* const pixelShader = dynamic_cast<PixelShader*>(m_ComputeShaders[2].get());

	if (pixelShader)
	{
		pixelShader->Use();
		pixelShader->SetPixelShaderParameters(PixelShaderParameters{pixelData.data(), pixelSize, sizeof(m_pixels)});
		pixelShader->SetInt("screenWidth", SCREEN_WIDTH);
		//pixelShader->SetInt("screenHeight", SCREEN_HEIGHT);

		pixelShader->Dispatch(surface->pixels, 0);
	}
}

glm::vec3 Camera::GetPosition() const
{
	return m_position;
}

void Camera::SetPosition(glm::vec3& newPosition)
{
	m_position = newPosition;
}

glm::vec3 Camera::GetCameraForward() const
{
	return m_forward;
}

glm::vec3 Camera::GetCameraUp() const
{
	return m_up;
}

glm::vec3 Camera::GetCameraRight() const
{
	return m_right;
}

glm::vec3 Camera::GetCameraLowerLeft() const
{
	return m_position - (m_right * 0.5f) - (m_up * 0.5f) - m_forward;
}

float Camera::GetCameraSpeed() const
{
	return m_speed;
}

float Camera::GetPitch() const
{
	return m_pitch;
}

void Camera::SetPitch(float pitch)
{
	m_pitch = pitch;
}

float Camera::GetYaw() const
{
	return m_yaw;
}

void Camera::SetYaw(float yaw)
{
	m_yaw = yaw;
}

#include "Camera.h"

Camera::Camera(glm::vec3& position, glm::vec3& targetLoc, glm::vec3& worldUp, float speed, float pitch, float yaw)
{
	m_position = position;
	m_direction = glm::normalize(m_position - targetLoc);

	m_right = glm::normalize(glm::cross(worldUp, m_direction));
	m_up = glm::cross(m_direction, m_right);
	m_forward = glm::vec3(0.0f, 0.0f, -1.0f);

	m_speed = speed;

	m_pitch = pitch;
	m_yaw = yaw;
}

Camera::~Camera()
{
	m_ComputeShader.reset();
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

	m_forward = glm::normalize(m_direction);
}

void Camera::Draw(float delta)
{
	if (m_ComputeShader)
	{
		m_ComputeShader->Use();
		//m_ComputeShader->SetFloat("delta", delta);
		//m_ComputeShader->SetVec3("cameraLowerLeft", GetCameraLowerLeft());
		//m_ComputeShader->SetVec3("cameraRight", m_right);
		//m_ComputeShader->SetVec3("cameraUp", m_up);
		//m_ComputeShader->SetVec3("cameraPos", m_position);
		m_ComputeShader->SetInt("screenWidth", SCREEN_WIDTH);
		m_ComputeShader->SetInt("screenHeight", SCREEN_HEIGHT);

		//glm::uvec2 dims= { SCREEN_WIDTH , SCREEN_HEIGHT }

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_pixelBuffer);
		glDispatchCompute(SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(PixelData), (GLvoid*)m_PixelData);
	}
}

glm::vec3 Camera::GetPosition() const
{
	return m_position;
}

void Camera::SetPosition(glm::vec3 newPosition)
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

void Camera::InitShader(const char* path)
{
	m_ComputeShader = std::make_unique<ComputeShader>(path);

	glGenBuffers(1, &m_pixelBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_pixelBuffer);
	glBufferData(GL_ARRAY_BUFFER, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(PixelData), 0, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void* Camera::GetPixelData()
{
	return &m_PixelData;
}

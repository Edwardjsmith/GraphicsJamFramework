#include "Camera.h"

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

#if RAYTRACER 1
#else
	InitRasterObjects();
#endif
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

	m_right = glm::normalize(glm::cross(GWorldUp, m_direction));
	m_up = glm::normalize(glm::cross(m_direction, m_right));

	m_forward = glm::normalize(m_direction);

	m_projection = glm::perspective(glm::radians(60.0f), static_cast<float>(SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 10.0f);
	m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
}

const int GDispatchX = SCREEN_WIDTH / 16;
const int GDispatchY = SCREEN_HEIGHT / 16;

void Camera::Draw(float delta)
{
	if (m_ComputeShader) 
	{
		m_ComputeShader->Use();

		m_ComputeShader->SetInt("screenWidth", SCREEN_WIDTH);
		m_ComputeShader->SetInt("screenHeight", SCREEN_HEIGHT);

#if RAYTRACER 1
		m_ComputeShader->SetVec3("cameraPos", m_position);
		m_ComputeShader->SetMatrix("inverseProjection", glm::inverse(m_projection));
		m_ComputeShader->SetMatrix("inverseView", glm::inverse(m_view));
#else
		m_ComputeShader->SetMatrix("ProjectionView", m_projection * m_view);

		for (int i = 0; i < m_ObjectTransforms.size(); ++i)
		{
			std::string uniformName = "objectTransform[" + std::to_string(i) + "]";
			m_ComputeShader->SetMatrix(uniformName, m_ObjectTransforms[i]);
		}
#endif


		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_pixelBuffer);
		glDispatchCompute(GDispatchX, GDispatchY, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(m_PixelData), (GLvoid*)m_PixelData);

		glDrawElements(GL_TRIANGLES, 0, 0, 0);
	}
}

glm::vec3 Camera::GetPosition() const
{
	return m_position;
}

void Camera::SetPosition(glm::vec3& newPosition)
{
	m_position = newPosition;

	//std::string cameraPosStr = "Camera pos: X " +
	//	std::to_string(m_position.x) + 
	//	", Y " + std::to_string(m_position.y) + 
	//	", Z " + std::to_string(m_position.z);

	//printf(cameraPosStr.c_str());
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

ProcessState Camera::InitShader(const char* path)
{
	m_ComputeShader = std::make_unique<ComputeShader>();

	if (m_ComputeShader->CompileShader(path) != ProcessState::OKAY)
	{
		return ProcessState::NOT_OKAY;
	}

	glGenBuffers(1, &m_pixelBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_pixelBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_PixelData), 0, GL_DYNAMIC_DRAW);

#if RAYTRACER 0
	glGenBuffers(1, &m_DepthBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_DepthBuffer);
	glBufferData(GL_ARRAY_BUFFER, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(float), 0, GL_DYNAMIC_DRAW);
#endif

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return ProcessState::OKAY;
}

const void* Camera::GetPixelData() const
{
	return &m_PixelData;
}


void Camera::InitRasterObjects()
{
	const glm::mat4 identity(1.0f);

	glm::mat4 Model0 = glm::translate(identity, glm::vec3(0.0f, 0.0f, -1.0f));
	Model0 = glm::rotate(Model0, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_ObjectTransforms.push_back(Model0);

	glm::mat4 Model1 = glm::translate(identity, glm::vec3(-5.0, 0.0f, -2.0f));
	Model1 = glm::rotate(Model1, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	m_ObjectTransforms.push_back(Model1);

	glm::mat4 Model2 = glm::translate(identity, glm::vec3(5.0, 0.0f, -3.0f));
	Model2 = glm::rotate(Model2, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_ObjectTransforms.push_back(Model2);
}


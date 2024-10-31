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
}

Camera::~Camera()
{
	m_ComputeShader[0].reset();
	m_ComputeShader[1].reset();
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

	m_ObjTransform = glm::rotate(m_ObjTransform, delta, glm::vec3(0.0f, 1.0f, 0.0f));
}

#if RAYTRACER 1
const int GDispatchX = SCREEN_WIDTH / 16;
const int GDispatchY = SCREEN_HEIGHT / 16;
#else

int GDispatchX = 0;
int GDispatchY = 0;

std::vector<VertexInput> GVertexData;
std::vector<uint32_t> GIndexData;
std::vector<int> GVisibleIndexData;
#endif

void Camera::Draw()
{
	if (m_ComputeShader[0])
	{
		m_ComputeShader[0]->Use();

		const unsigned int programID = m_ComputeShader[0]->GetID();

		int index = 0;
		for (GLint buffer : m_ComputeShader[0]->GetShaderBuffers())
		{
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index++, buffer);

		}

		GVisibleIndexData.clear();
		GVisibleIndexData.resize(GIndexData.size() / 3);

		glDispatchCompute((GVisibleIndexData.size() / 3), 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, 4);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (sizeof(int) * GIndexData.size()) / 3, (GLvoid*)GVisibleIndexData.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		for (int i = GVisibleIndexData.size() - 1; i > 0; --i)
		{
			if (GVisibleIndexData[i] < 0)
			{
				GVisibleIndexData.erase(GVisibleIndexData.begin() + i);
			}
		}
	}

	if (m_ComputeShader[1])
	{
		m_ComputeShader[1]->Use();

		m_ComputeShader[1]->SetInt("screenWidth", SCREEN_WIDTH);
		m_ComputeShader[1]->SetInt("screenHeight", SCREEN_HEIGHT);

		const unsigned int programID = m_ComputeShader[1]->GetID();

		m_ComputeShader[1]->SetMatrix("ProjectionViewModel", m_projection * m_view * m_ObjTransform);

		int index = 0;
		for (GLint buffer : m_ComputeShader[1]->GetShaderBuffers())
		{
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index++, buffer);
		}

		glDispatchCompute(GDispatchX, GDispatchY, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, 8);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(m_PixelData), (GLvoid*)m_PixelData);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	/*if (m_ComputeShader) 
	{
		m_ComputeShader->Use();

		m_ComputeShader->SetInt("screenWidth", SCREEN_WIDTH);
		m_ComputeShader->SetInt("screenHeight", SCREEN_HEIGHT);

		const unsigned int programID = m_ComputeShader->GetID();

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_PixelBuffer);

#if RAYTRACER 1
		m_ComputeShader->SetVec3("cameraPos", m_position);
		m_ComputeShader->SetMatrix("inverseProjection", glm::inverse(m_projection));
		m_ComputeShader->SetMatrix("inverseView", glm::inverse(m_view));
#else
		m_ComputeShader->SetMatrix("ProjectionViewModel", m_projection * m_view * m_ObjTransform);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_DepthBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_VertexBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_IndexBuffer);
#endif

		glDispatchCompute(GDispatchX, GDispatchY, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_PixelBuffer);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(m_PixelData), (GLvoid*)m_PixelData);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}*/
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

ProcessState Camera::InitShader(const char* path, int index)
{
	m_ComputeShader[index] = std::make_unique<ComputeShader>();

	if (m_ComputeShader[index]->CompileShader(path) != ProcessState::OKAY)
	{
		return ProcessState::NOT_OKAY;
	}

#if RAYTRACER 1
#else

	GLuint buffer = 0;
	std::vector<GLint>& Buffers = m_ComputeShader[index]->GetShaderBuffers();

	//Depth buffer
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * (SCREEN_WIDTH * SCREEN_HEIGHT), 0, GL_DYNAMIC_DRAW);
	Buffers.push_back(buffer);

	//Vertex buffer
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VertexInput) * GVertexData.size(), GVertexData.data(), GL_DYNAMIC_READ);
	Buffers.push_back(buffer);

	//Index buffer
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * GIndexData.size(), GIndexData.data(), GL_DYNAMIC_READ);
	Buffers.push_back(buffer);
#endif

	//Pixel buffer
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_PixelData), 0, GL_DYNAMIC_DRAW);
	Buffers.push_back(buffer);

	//Visible index
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (sizeof(int) * GIndexData.size()) / 3, 0, GL_DYNAMIC_DRAW);
	Buffers.push_back(buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	return ProcessState::OKAY;
}

const void* Camera::GetPixelData() const
{
	return &m_PixelData;
}

void Camera::ResetPixelData()
{
	std::fill(std::begin(m_PixelData), std::end(m_PixelData), PixelData());
}

ProcessState Camera::InitRasterShader(const char* path)
{
	return InitShader(path, 1);
}

ProcessState Camera::InitTriangleFilterShader(const char* path)
{
	m_ComputeShader[0] = std::make_unique<ComputeShader>();

	if (m_ComputeShader[0]->CompileShader(path) != ProcessState::OKAY)
	{
		return ProcessState::NOT_OKAY;
	}

#if RAYTRACER 1
#else

	GLuint buffer = 0;
	std::vector<GLint>& Buffers = m_ComputeShader[0]->GetShaderBuffers();

	//Depth
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * (SCREEN_WIDTH * SCREEN_HEIGHT), 0, GL_DYNAMIC_DRAW);
	Buffers.push_back(buffer);

	//Vertex
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VertexInput) * GVertexData.size(), GVertexData.data(), GL_DYNAMIC_READ);
	Buffers.push_back(buffer);

	//Index
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * GIndexData.size(), GIndexData.data(), GL_DYNAMIC_READ);
	Buffers.push_back(buffer);

	//Visible index
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (sizeof(int) * GIndexData.size()) / 3, 0, GL_DYNAMIC_DRAW);
	Buffers.push_back(buffer);


#endif

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	return ProcessState::OKAY;
}



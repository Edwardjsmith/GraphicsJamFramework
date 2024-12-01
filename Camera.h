#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "SoftwareRasterizer.h"
#include "TriangleFilter.h"
#include <SDL_surface.h>

extern std::vector<VertexInput> GVertexData;
extern std::vector<uint32_t> GIndexData;

extern int GDispatchX, GDispatchY;

static glm::vec3 GWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

class Camera
{
public:

	Camera(glm::vec3& position, glm::vec3& targetLoc, glm::vec3& worldUp, float speed, float pitch, float yaw);
	~Camera();

	void Update(float delta);
	void Draw(SDL_Surface* const surface);

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3& newPosition);

	glm::vec3 GetCameraForward() const;
	glm::vec3 GetCameraUp() const;
	glm::vec3 GetCameraRight() const;

	glm::vec3 GetCameraLowerLeft() const;

	float GetCameraSpeed() const;

	float GetPitch() const;
	void SetPitch(float pitch);

	float GetYaw() const;
	void SetYaw(float yaw);

	const void* GetPixels() const { return &m_pixels; }

	template<typename T>
	ProcessState CompileShader(const char* computePath)
	{
		std::unique_ptr<T> Shader(new T());

		if (Shader->CompileShader(computePath) != ProcessState::NOT_OKAY)
		{
			Shader->GenerateBuffers();
			m_ComputeShaders.push_back(std::move(Shader));
			return ProcessState::OKAY;
		}

		Shader.release();
		return ProcessState::NOT_OKAY;
	}

	ProcessState InitRasterShader(const char* path);

	ProcessState InitTriangleFilterShader(const char* path);

private:

	uint32_t m_pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

	glm::vec3 m_position;
	glm::vec3 m_direction;

	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_forward;

	glm::mat4 m_projection;
	glm::mat4 m_view;

	float m_speed;

	float m_pitch, m_yaw;

	std::vector<std::unique_ptr<ComputeShader>> m_ComputeShaders;

	glm::mat4 m_ObjTransform = glm::mat4(1.0f);

	TriangleData m_TriangleData[512];
};

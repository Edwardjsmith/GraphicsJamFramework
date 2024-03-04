#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "ComputeShader.h"

const unsigned int SCREEN_WIDTH = 640;
const unsigned int SCREEN_HEIGHT = 480;

class Camera
{
public:

	Camera(glm::vec3& position, glm::vec3& targetLoc, glm::vec3& worldUp, float speed, float pitch, float yaw);
	~Camera();

	void Update(float delta);
	void Draw(float delta);

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 newPosition);

	glm::vec3 GetCameraForward() const;
	glm::vec3 GetCameraUp() const;
	glm::vec3 GetCameraRight() const;

	glm::vec3 GetCameraLowerLeft() const;

	float GetCameraSpeed() const;

	float GetPitch() const;
	void SetPitch(float pitch);

	float GetYaw() const;
	void SetYaw(float yaw);

	void InitShader(const char* path);

	void* GetPixelData();

private:

	glm::vec3 m_position;
	glm::vec3 m_direction;

	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_forward;

	float m_speed;

	float m_pitch, m_yaw;

	std::unique_ptr<ComputeShader> m_ComputeShader = nullptr;

	glm::vec4 m_PixelData[SCREEN_WIDTH * SCREEN_HEIGHT];

	GLuint m_pixelBuffer = 0;
};


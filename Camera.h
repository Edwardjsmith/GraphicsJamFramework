#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "ComputeShader.h"

#ifndef RAYTRACER
	#define RAYTRACER 0;
#endif

const unsigned int SCREEN_WIDTH = 640;
const unsigned int SCREEN_HEIGHT = 480;

#if RAYTRACER 1
#else
#pragma pack(push, 1)
struct VertexInput
{
	glm::vec4   Pos;
	glm::vec3   Normal;
	GLbyte padding1[4];
	glm::vec2	UV;
	GLbyte padding2[8];
};
#pragma pack( pop )

struct TriangleData
{
	VertexInput VertData[3];
	int bInitialized = 0;
	GLbyte padding0[12];
};

extern std::vector<VertexInput> GVertexData;
extern std::vector<uint32_t> GIndexData;
extern std::vector<TriangleData> GVisibleTriangleData;

extern int GDispatchX, GDispatchY;
#endif

struct PixelData
{
	PixelData()
	{
		color = glm::vec4(0);
	}

	glm::vec4 color;
};

static glm::vec3 GWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

class Camera
{
public:

	Camera(glm::vec3& position, glm::vec3& targetLoc, glm::vec3& worldUp, float speed, float pitch, float yaw);
	~Camera();

	void Update(float delta);
	void Draw();

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

	ProcessState InitShader(const char* path, int index);

	const void* GetPixelData() const;

	void ResetPixelData();

	ProcessState InitRasterShader(const char* path);

	ProcessState InitTriangleFilterShader(const char* path);

private:

	glm::vec3 m_position;
	glm::vec3 m_direction;

	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_forward;

	glm::mat4 m_projection;
	glm::mat4 m_view;

	float m_speed;

	float m_pitch, m_yaw;

	std::unique_ptr<ComputeShader> m_ComputeShader[2] = { nullptr, nullptr };

	PixelData m_PixelData[SCREEN_WIDTH * SCREEN_HEIGHT];

#if RAYTRACER 1
#else
#endif

	glm::mat4 m_ObjTransform = glm::mat4(1.0f);
};


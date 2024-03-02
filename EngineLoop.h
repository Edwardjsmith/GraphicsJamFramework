#pragma once
#include <memory>
#include <SDL.h>
#include "ComputeShader.h"
#undef main

enum class ProcessState
{
	NOT_OKAY = -1,
	OKAY = 0
};

const unsigned int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;

class EngineLoop
{
public:

	ProcessState Init();
	ProcessState Draw();
	ProcessState Update();

	void Cleanup();

	bool ShutdownPending() const;

private:

//SDL stuff
	SDL_Window* m_SDLWindow = nullptr;
	SDL_Surface* m_mainSurface = nullptr;

	SDL_Event SDLEvent;

	ProcessState SDLInit();
	void SDLCleanup();

//OpenGL stuff
	ProcessState OpenGLInit(const char* computePath);
	void OpenGLCleanup();

	GLFWwindow* m_OpenGLWindow = nullptr;

	std::unique_ptr<ComputeShader> m_ComputeShader = nullptr;

	GLuint m_pixelBuffer = 0;
	glm::vec4 m_pixelData[SCREEN_WIDTH * SCREEN_HEIGHT];
};


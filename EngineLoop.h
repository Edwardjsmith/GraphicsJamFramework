#pragma once
#include <SDL.h>
#include "Camera.h"
#undef main

enum class ProcessState
{
	NOT_OKAY = -1,
	OKAY = 0
};

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

	std::unique_ptr<Camera> m_Camera = nullptr;
};


#pragma once
#include <SDL.h>
#include "Camera.h"
#undef main

class EngineLoop
{
public:

	ProcessState Init();
	ProcessState Draw();
	ProcessState Update(float delta);

	void Cleanup();

	bool ShutdownPending() const;

private:

//SDL stuff
	SDL_Window* m_SDLWindow = nullptr;
	SDL_Surface* m_mainSurface = nullptr;

	SDL_Event SDLEvent;

	ProcessState SDLInit(const char* computePath);
	void SDLCleanup();

#if RAYTRACER 1
#else
	//Asset loading
	ProcessState LoadAssets(const char* pathName);
#endif

//OpenGL context
	SDL_GLContext context = nullptr;

	std::unique_ptr<Camera> m_Camera = nullptr;

//Mouse stuff
	float m_lastMouseX = SCREEN_WIDTH * 0.5, m_lastMouseY = SCREEN_HEIGHT * 0.5;
	const float m_sensitivity = 0.5f;

	void HandleMouseMovement(float& offsetX, float& offsetY);
};


#include "EngineLoop.h"

std::unique_ptr<EngineLoop> engineLoopInstance = nullptr;

int main()
{
	engineLoopInstance = std::make_unique<EngineLoop>();

	ProcessState currentState = ProcessState::OKAY;

	currentState = engineLoopInstance->Init();

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;

	double delta = 0.0f;

	while (!engineLoopInstance->ShutdownPending() && currentState == ProcessState::OKAY)
	{
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		delta = static_cast<double>((NOW - LAST) / static_cast<double>(SDL_GetPerformanceFrequency()));

		currentState = engineLoopInstance->Update(static_cast<float>(delta));

		if (currentState != ProcessState::OKAY)
		{
			break;
		}

		currentState = engineLoopInstance->Draw();
	}

	engineLoopInstance->Cleanup();
	engineLoopInstance.reset();

	return static_cast<int>(currentState);
}
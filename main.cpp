#include "EngineLoop.h"

std::unique_ptr<EngineLoop> engineLoopInstance = nullptr;

int main()
{
	engineLoopInstance = std::make_unique<EngineLoop>();

	ProcessState currentState = ProcessState::OKAY;

	currentState = engineLoopInstance->Init();

	while (!engineLoopInstance->ShutdownPending() && currentState == ProcessState::OKAY)
	{
		currentState = engineLoopInstance->Update();

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
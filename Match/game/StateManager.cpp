#include "StateManager.h"
StateManager* StateManager::GetInstance()
{
	static StateManager statemanager;
	return &statemanager;
}

void StateManager::PushState(States* pState)
{
	gameStates.push_back(pState);
	gameStates.back()->Enter();
}

void StateManager::ChangeState(States* pState)
{
	if (!gameStates.empty())
	{
		if (gameStates.back()->getStateID() == pState->getStateID())
		{
			return; // do nothing
		}
		if (gameStates.back()->Exit())
		{
			delete gameStates.back();
			gameStates.pop_back();
		}
	}
		gameStates.push_back(pState);
		gameStates.back()->Enter();
}

void StateManager::PopState()
{
	if (!gameStates.empty())
	{
		if (gameStates.back()->Exit())
		{
			delete gameStates.back();
			gameStates.pop_back();
		}
	}
}

void StateManager::Render()
{
	if (!gameStates.empty())
	{
		gameStates.back()->Render();
	}
}

void StateManager::Update(double deltams)
{
	if (!gameStates.empty())
	{
		gameStates.back()->Update(deltams);
	}
}

void StateManager::PollEvent(const SDL_Event& ev)
{
	if (!gameStates.empty())
	{
		gameStates.back()->PollEvent(ev);
	}
}

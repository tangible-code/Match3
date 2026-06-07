#pragma once
#include "States.h"
#include <vector>
class StateManager
{
public:
	static StateManager* GetInstance();
	void PushState(States* pState);
	void ChangeState(States* pState);
	void PopState();
	void Render();
	void Update(double deltams);
	void PollEvent(const SDL_Event& ev);
private:
	StateManager() {};
	std::vector<States*> gameStates;
};


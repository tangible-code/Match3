#pragma once
#include <vector>
#include "Panel.h"

class Ui
{
public:
	bool Handle(const SDL_Event& e);
	void Step();
	//
	void Pop(Panel* panel);
	void Push(Panel* panel);
	void PopThrough(Panel* panel);
	void Draw();

	void Reset();

	bool IsTop(Panel* panel);

	bool Empty();

private:
	void PushAndPop();
private:

	std::vector<Panel*> stack;
	std::vector<Panel*> toPop;
	std::vector<Panel*> toPush;

};
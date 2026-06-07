#include "Ui.h"
#include "../Application.h"
bool Ui::Handle(const SDL_Event& e)
{
	bool handled = false;
	std::vector<Panel*>::iterator it = stack.end();

	while (it != stack.begin() && !handled)
	{
		--it;
		if (std::count(toPop.begin(), toPop.end(), *it))
		{
			continue;
		}

		if (e.type == SDL_MOUSEMOTION)
		{
			
			if (e.motion.state & SDL_BUTTON(1))
				handled = (*it)->Drag(
					e.motion.xrel,
					e.motion.yrel);
			else
				handled = (*it)->Hover(
					e.motion.x,
					e.motion.y);
					
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			int x = e.button.x;
			int y = e.button.y;

			if (e.button.button == 1)
			{
				handled = (*it)->ZoneClick({ x, y });
				if (!handled)
					handled = (*it)->Click(x, y, e.button.clicks);
			}
			else if (e.button.button == 3)
				handled = (*it)->RClick(x, y);
		}
		else if (e.type == SDL_MOUSEBUTTONUP)
		{
			int x = e.button.x;
			int y = e.button.y;

			handled = (*it)->Release(x, y);
		}
		else if (e.type == SDL_MOUSEWHEEL)
			handled = (*it)->Scroll(e.wheel.x, e.wheel.y);
		else if (e.type == SDL_KEYDOWN)
		{
			//Command command(event.key.keysym.sym);
			handled = (*it)->KeyDown(e.key.keysym.sym, e.key.keysym.mod, !e.key.repeat);
		}
		else if (e.type == SDL_KEYUP)
		{
			handled = (*it)->KeyUp(e.key.keysym.sym, e.key.keysym.mod, !e.key.repeat);
		}

		// If this panel does not want anything below it to receive events, do
		// not let this event trickle further down the stack.
		if ((*it)->TrapAllEvent())
			break;
	}

	PushAndPop();

	return handled;
}

void Ui::Step()
{
	PushAndPop();

	for (Panel* p : stack)
	{
		p->Step();
	}
}

void Ui::Pop(Panel* panel)
{
	toPop.push_back(panel);
}

void Ui::Push(Panel* panel)
{
	panel->SetUi(this);
	toPush.push_back(panel);
}

void Ui::PopThrough(Panel* panel)
{
	for (auto it = stack.rbegin(); it != stack.rend(); ++it)
	{
		toPop.push_back(*it);
		if (*it == panel)
			break;
	}
}

void Ui::Draw()
{
	for (Panel* it : stack)
		it->ClearZones();

	// Find the topmost full-screen panel. Nothing below that needs to be drawn.
	std::vector<Panel*>::const_iterator it = stack.end();
	while (it != stack.begin())
		if ((*--it)->IsFullScreen())
			break;

	for (; it != stack.end(); ++it)
		(*it)->Draw();
}

void Ui::Reset()
{
	stack.clear();
	toPush.clear();
	toPop.clear();
}

bool Ui::IsTop(Panel* panel)
{
	return (!stack.empty() && stack.back() == panel);
}


bool Ui::Empty()
{
	return stack.empty() && toPush.empty();
}

void Ui::PushAndPop()
{
	for (Panel* panel : toPush)
		if (panel)
			stack.push_back(panel);
	toPush.clear();

	// These panels should be popped but not deleted (because someone else
	// owns them and is managing their creation and deletion).
	for (Panel* panel : toPop)
	{
		for (auto it = stack.begin(); it != stack.end(); ++it)
			if (*it == panel)
			{
				it = stack.erase(it);
				break;
			}
	}
	toPop.clear();
}

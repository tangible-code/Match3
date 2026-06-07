#include "Panel.h"

void Panel::Step()
{
	//blank
}

bool Panel::IsFullScreen()
{
	return isFullScreen;
}

bool Panel::TrapAllEvent()
{
	return trapAllEvent;
}

bool Panel::IsInterruptile()
{
	return isInterruptile;
}

void Panel::ClearZones()
{
	zones.clear();
}

void Panel::AddZone(SDL_Rect rect,const std::function<void()> fun)
{
	zones.emplace_front(Zone({rect}, fun));
}

bool Panel::ZoneClick(glm::vec2 point)
{

	for (Zone zone : zones)
	{
		if (zone.Contain(point))
		{
			zone.Click();
			return true;
		}
	}
	return false;
}

bool Panel::KeyDown(SDL_Keycode key, Uint16 mod, bool isNewPress)
{
	return false;
}

bool Panel::KeyUp(SDL_Keycode key, Uint16 mod, bool isNewPress)
{
	return false;
}

bool Panel::Click(int x, int y, int clicks)
{
	return false;
}

bool Panel::RClick(int x, int y)
{
	return false;
}

bool Panel::Hover(int x, int y)
{
	return false;
}

bool Panel::Drag(int dx, int dy)
{
	return false;
}

bool Panel::Release(int x, int y)
{
	return false;
}

bool Panel::Scroll(int dx, int dy)
{
	return false;
}

void Panel::SetIsFullScreen(bool value)
{
	isFullScreen = value;
}

void Panel::SetTrapAllEvent(bool value)
{
	trapAllEvent = value;
}

void Panel::SetIsInterruptile(bool value)
{
	isInterruptile = value;
}

Ui* Panel::GetUi()
{
	return ui;
}

void Panel::SetUi(Ui* ui)
{
	this->ui = ui;
}

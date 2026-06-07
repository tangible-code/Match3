#pragma once
#include <list>
#include <string>
#include <SDL.h>
#include <functional>
#include <glm/glm.hpp>
#include "Rect.h"

class Ui;
class Panel
{
public:
	virtual ~Panel() = default;

	virtual void Step();

	virtual void Draw() = 0;

	bool IsFullScreen();
	bool TrapAllEvent();
	bool IsInterruptile();

	void ClearZones();

	void AddZone(SDL_Rect rect,const std::function<void()> fun);

	bool ZoneClick(glm::vec2 point);


protected:
	virtual bool KeyDown(SDL_Keycode key, Uint16 mod, bool isNewPress);
	virtual bool KeyUp(SDL_Keycode key, Uint16 mod, bool isNewPress);
	virtual bool Click(int x, int y, int clicks);
	virtual bool RClick(int x, int y);
	virtual bool Hover(int x, int y);
	virtual bool Drag(int dx, int dy);
	virtual bool Release(int x, int y);
	virtual bool Scroll(int dx, int dy);

	void SetIsFullScreen(bool value);
	void SetTrapAllEvent(bool value);
	void SetIsInterruptile(bool value);

	Ui* GetUi();
private:
	class Zone : public Rect
	{
	public:
		Zone(Rect rect,const std::function<void()> fun)
			: Rect(rect), fun(fun)
		{}

		void Click() const { fun(); }
	private:
		const std::function<void()> fun;
	};

private:
	void SetUi(Ui* ui);
private:
	Ui* ui = nullptr;

	bool isFullScreen = false;
	bool trapAllEvent = false;
	bool isInterruptile = false;

	std::list<Zone> zones;

	friend class Ui;
};
#pragma once
#include <SDL.h>

class Physics
{
public:


	Physics* GetInstance() 
	{
		static Physics instance;
		return &instance;
	}


	bool RectVsRect(SDL_Rect& r1, SDL_Rect& r2) {
		return r1.x < r2.x + r2.w &&
			r1.x + r1.w > r2.x &&
			r1.y < r2.y + r2.h &&
			r1.h + r1.y > r2.y;
	}

	bool CircleVsRect(float cx, float cy, float cr, SDL_Rect& rect)
	{
		float testx = cx;
		float testy = cy;

		if (cx < float(rect.x))
		{
			testx = (float)rect.x;
		}
		else if (cx > (float)(rect.x + rect.w))
		{
			testx = float(rect.x + rect.w);
		}

		if (cy < float(rect.y))
		{
			testy = float(rect.y);
		}
		else if (cy > float(rect.y + rect.h))
		{
			testy = float(rect.y + rect.h);
		}

		float distx = cx - testx;
		float disty = cy - testy;
		float distance = sqrtf((distx * distx) + (disty * disty));

		if (distance <= cr)
		{
			return true;
		}
		
		return false;
	}
private:
	Physics() {}
};
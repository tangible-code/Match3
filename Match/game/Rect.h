#pragma once
#include <glm/glm.hpp>
#include <SDL.h>

class Rect
{
public:
	bool Overlap(SDL_Rect& r)
	{
		return rect.x < r.x + r.w &&
			rect.x + rect.w > r.x &&
			rect.y < r.y + r.h &&
			rect.h + rect.y > r.y;
	}
	bool Contain(SDL_Rect& r)
	{
		return rect.x <= r.x && rect.x + rect.w > r.x + r.w && rect.y <= r.y && rect.y + rect.h > r.y + r.h;
	}
	bool Contain(glm::vec2 point)
	{
		return rect.x <= point.x && rect.x + rect.w >= point.x && rect.y <= point.y && rect.y + rect.h >= point.y;
	}

	bool Contain(glm::vec2 position, float radius)
	{
		return (position.x + radius < rect.x + rect.w) && (position.x - radius > rect.x) && (position.y - radius > rect.y) && (position.y + radius < rect.y + rect.h);
	}

	bool Overlap(glm::vec2 position, float radius)
	{
		float testX = position.x;
		float testY = position.y;

		if (position.x < rect.x)
		{
			testX = (float)rect.x;
		}
		else if (position.x > rect.x + rect.w)
		{
			testX = float(rect.x + rect.w);
		}
		else if (position.y < rect.y)
		{
			testY = (float)rect.y;
		}
		else if (position.y > rect.y + rect.h)
		{
			testY = float(rect.y + rect.h);
		}

		float distX = position.x - testX;
		float distY = position.y - testY;

		float distance = glm::sqrt((distX * distX) + (distY * distY));

		if (distance <= radius)
		{
			return true;
		}

		return false;
	}

public:
	SDL_Rect rect;

};


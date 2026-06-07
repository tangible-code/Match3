#pragma once
#include <glm/glm.hpp>
#include <SDL.h>

class Camera
{
public:
	Camera();
	void Attach(glm::vec2* pos);
	void SetZoom(float value);
	void Update(double deltams);
	void SetBoundary(float left, float right, float up, float down);
	float GetZoom();
public:
	glm::vec2* position;
	glm::vec2 offsets;
	SDL_Rect camera;
private:
	float zoom;
	glm::vec2 min, max;
};
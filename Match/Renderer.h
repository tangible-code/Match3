#pragma once
#include <glm/glm.hpp>
#include <SDL.h>
#include <glad/glad.h>
class Renderer
{
public:
	Renderer();
	bool Initialize(glm::mat4 proj,SDL_Window* win);
	void Pre_Renderer();
	void Post_Renderer();
	void SetClearColor(glm::vec4 c);

	bool Quit();

	glm::mat4 projection;
	glm::vec4 color;
	SDL_Window* window;
	SDL_GLContext glContext;
};


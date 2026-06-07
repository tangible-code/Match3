#include "Renderer.h"

Renderer::Renderer()
{
	projection = glm::mat4(1.0f);
	color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

bool Renderer::Initialize(glm::mat4 proj ,SDL_Window* win)
{
	projection = proj;
	window = win;

	if (window == nullptr)
	{
		return false;
	}

	glContext = SDL_GL_CreateContext(window);

	if (glContext == nullptr) {
		printf("Context didnt created\n");
		return false;
	}
	else {
		printf("context created\n");
	}


	SDL_GL_MakeCurrent(window, glContext);
	SDL_GL_SetSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		printf("couldnt intialize glad\n");
		return false;
	}
	else {
		printf("glad initalized\n");
	}

	return true;
}

void Renderer::Pre_Renderer()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(color.x,color.y,color.z,color.w);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Post_Renderer()
{
	SDL_GL_SwapWindow(window);
}

void Renderer::SetClearColor(glm::vec4 c)
{
	color = c;
}

bool Renderer::Quit()
{
	SDL_GL_DeleteContext(glContext);
	return true;
}

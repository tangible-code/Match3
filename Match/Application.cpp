#include "Application.h"
#include <glm/gtc/matrix_transform.hpp>
#include <windows.h>

int Application::width;
int Application::height;
SDL_Window* Application::window = nullptr;
Renderer* Application::g_renderer = nullptr;
GameLogic* Application::g_logic = nullptr;
Uint64 Application::previousTick;
bool Application::quit = false;

std::filesystem::path Application::savePath;
bool Application::Initialize(const char* title, int Width, int Height)
{

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL couldnt initialize\n");
		return false;
	}
	else {
		printf("succesfully initalized sdl\n");
	}


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	width = Width;
	height = Height;

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	if (window == nullptr) {
		printf("SDL Window didnt created\n");
		SDL_Quit();
		return false;
	}
	else {
		printf("Window created\n");
	}

	if (!CheckRam(20))
	{
		return false;
	}

	g_renderer = new Renderer();


	if (!g_renderer->Initialize(glm::ortho(0.0f, float(width), float(height), 0.0f, -1.0f, 1.0f), window))
	{
		printf("renderer init failed\n");
		return false;
	}
	else {
		printf("renderer created\n");
	}


	glViewport(0, 0, width, height);

	Audio::Init();

	g_logic = new GameLogic();

	if (g_logic == nullptr)
	{
		return false;
	}

	if (!g_logic->Initialize())
	{
		printf("logic failed\n");
		return false;
	}

	return true;
}

void Application::Run()
{
	while (!quit)
	{
		Uint64 currentTick = SDL_GetTicks64();
		double deltams = double(currentTick - previousTick) / 1000.0f;
		previousTick = SDL_GetTicks64();

		SDL_Event e;

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				printf("exiting\n");
				Quit();
				quit = true;
			}

			if (e.type == SDL_WINDOWEVENT_RESIZED) {
				SDL_GetWindowSize(window, &width, &height);
				glViewport(0, 0, width, height);
				g_renderer->projection = glm::ortho(0.0f, float(width), float(height), 0.0f, -1.0f, 1.0f);
			}

			if (e.type == SDL_KEYDOWN) {
				/*if (e.key.keysym.sym == SDLK_ESCAPE) {
					printf("hotkey quiting\n");
					Quit();
					quit = true;
				}*/


			}

			g_logic->ProcEvent(e);

		}

		Audio::Step();
		BatchRenderer::Update(1.0 / deltams);

		g_logic->Update(1.0 / deltams);

		g_renderer->Pre_Renderer();

		g_logic->Render();

		g_renderer->Post_Renderer();
	}
	Destroy();
}

bool Application::Quit()
{
	//quiting all subsystem and checking status not destroying sdl window
	g_logic->Quit();
	g_renderer->Quit();
	Audio::Quit();
	return true;
}

void Application::Destroy()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
	printf("Destroy\n");
}

bool Application::CheckRam(unsigned int size)
{
	///for windows

	MEMORYSTATUSEX t_status;
	t_status.dwLength = sizeof(t_status);
	GlobalMemoryStatusEx(&t_status);
	if (t_status.ullAvailPhys < MbToBytes(size)) {
		printf("Warning: Not Enough Memory\n");
		return false;
	}

	if (t_status.ullAvailVirtual < MbToBytes(size)) {
		printf("Warning: Not Enough VRAM\n");
		return false;
	}
	return true;
}

size_t Application::MbToBytes(unsigned int mb)
{
	return size_t(mb * (1024 * 1024));
}



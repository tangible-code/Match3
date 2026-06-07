#pragma once
#include "Renderer.h"
#include "GameLogic.h"
#include "Audio.h"
#include <filesystem>

class Application
{
public:
	static bool Initialize(const char* title, int Width, int Height);
	static void Run();
	static bool Quit();

	static void Destroy();


public:

	static int width, height;
	static SDL_Window* window;
	static Uint64 previousTick;
	static bool quit;
public:
	static Renderer* g_renderer;
	static GameLogic* g_logic;

private:
	static std::filesystem::path savePath;

	static bool CheckRam(unsigned int size);

	static size_t MbToBytes(unsigned int mb);
};


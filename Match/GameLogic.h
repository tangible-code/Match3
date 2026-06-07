#pragma once
#include "ResourceManager.h"
#include "FontRenderer.h"
#include "PrimitiveRenderer.h"
#include "AtlasRenderer.h"
#include <SDL.h>
#include "game/Camera.h"
#include <random>
#include "game/Ui.h"
#include "BatchRenderer.h"

class GameLogic
{
public:
	GameLogic();

	bool Initialize();

	void Update(double deltams);
	void Render();
	void ProcEvent(SDL_Event& event);

	bool Quit();

public:
	FontRenderer* f_Render;
	PrimitiveRenderer* p_Render;
	SpriteRenderer* a_Render;



	//debug
	double delta;

public:
	Camera* g_Camera;
	Ui* ui;
};


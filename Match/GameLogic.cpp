#include "GameLogic.h"
#include "Application.h"
#include "game/StateManager.h"
#include <iostream>


GameLogic::GameLogic()
{
    f_Render = nullptr;
    p_Render = nullptr;
    delta = 0.0;
    a_Render = nullptr;
    
}

bool GameLogic::Initialize()
{
    f_Render = new FontRenderer();
    if (f_Render == nullptr)
    {
        printf("Font Renderer could not initialize\n");
        return false;
    }

    p_Render = new PrimitiveRenderer();

    if (p_Render == nullptr)
    {
        printf("Primitive Renderer could not initialize\n");
        return false;
    }

    a_Render = new SpriteRenderer();

    if (a_Render == nullptr)
    {
        printf("Atlas Sprite Renderer could not initialize\n");
        return false;
    }

    BatchRenderer::Init();

    BatchRenderer::SetProjectionMat(Application::g_renderer->projection);
    f_Render->SetProjectionMat(Application::g_renderer->projection);
    p_Render->SetProjectionMat(Application::g_renderer->projection);
    a_Render->SetProjectionMat(Application::g_renderer->projection);


    ResourceManager::LoadFont("default", "Resources/font/segoeuisl.ttf",20);

    g_Camera = new Camera;
    ui = new Ui;

    //resorces
    ResourceManager::LoadTexture("background_frame", "Resources/image/BackTile_04.png");
    ResourceManager::LoadTexture("blue_background_frame", "Resources/image/BackTile_10.png");
    ResourceManager::LoadTexture("black_circle", "Resources/image/tileBlack_11.png");
    ResourceManager::LoadTexture("blue_diamond", "Resources/image/tileBlue_04.png");
    ResourceManager::LoadTexture("green_gogone", "Resources/image/tileGreen_05.png");
    ResourceManager::LoadTexture("grey_octagone", "Resources/image/tileGrey_06.png");
    ResourceManager::LoadTexture("orange_trigone", "Resources/image/tileOrange_18.png");
    ResourceManager::LoadTexture("pink_heart", "Resources/image/tilePink_12.png");
    ResourceManager::LoadTexture("red_square", "Resources/image/tileRed_01.png");
    ResourceManager::LoadTexture("yellow_star", "Resources/image/tileYellow_09.png");
    ResourceManager::LoadTexture("particle", "Resources/image/particleWhite_1.png");

    ResourceManager::LoadTexture("grey_panel", "Resources/image/grey_panel.png");

    ResourceManager::LoadFont("thick", "Resources/font/Kenney Thick.ttf", 20);
    ResourceManager::LoadFont("thick_big", "Resources/font/Kenney Thick.ttf", 24);
    ResourceManager::LoadFont("gluten", "Resources/font/Gluten-Medium.ttf", 100);

    ResourceManager::LoadTexture("hover", "Resources/image/yellow_button00.png");
    ResourceManager::LoadTexture("click", "Resources/image/yellow_button01.png");
    ResourceManager::LoadTexture("idle", "Resources/image/yellow_button02.png");

    ResourceManager::LoadTexture("bhover", "Resources/image/blue_button00.png");
    ResourceManager::LoadTexture("bclick", "Resources/image/blue_button01.png");
    ResourceManager::LoadTexture("bidle", "Resources/image/blue_button02.png");

    ResourceManager::LoadSound("click", "Resources/audio/click3.wav");
    ResourceManager::LoadSound("retro_beat", "Resources/audio/Retro Beat.wav");
    ResourceManager::LoadSound("hit", "Resources/audio/hit3.wav");
    ResourceManager::LoadSound("confirm", "Resources/audio/confirm_style_3_001.wav");


    StateManager::GetInstance()->PushState(new Menu);
   return true;
}

void GameLogic::Update(double deltams)
{
    delta = deltams;
   
    g_Camera->Update(deltams);
    ui->Step();

    StateManager::GetInstance()->Update(deltams);
}

void GameLogic::Render()
{
    StateManager::GetInstance()->Render();
    ui->Draw();
    //f_Render->RenderText({ 1.0f,4.0f }, std::to_string(delta), ResourceManager::GetFont("default"), { 1.0f,0.0f,0.0f });
}

void GameLogic::ProcEvent(SDL_Event& event)
{
    if(event.type == SDL_WINDOWEVENT_RESIZED)
    {
        f_Render->SetProjectionMat(Application::g_renderer->projection);
        BatchRenderer::SetProjectionMat(Application::g_renderer->projection);
    }
    ui->Handle(event);
    StateManager::GetInstance()->PollEvent(event);
}

bool GameLogic::Quit()
{
    StateManager::GetInstance()->PopState();
    ResourceManager::Clear();
    BatchRenderer::Destroy();
    delete ui;
    delete g_Camera;
    delete f_Render;
    delete p_Render;
    delete a_Render;
    return true;
}


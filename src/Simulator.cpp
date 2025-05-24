#include "../include/Simulator.h"
#include "../include/Event.h"
#include "../include/EventManager.h"
#include "../include/AssetManager.h"


void Simulator::Initialize(const WindowProperties &winProp)
{
    m_FrameDelay = 1.f/m_FPS * 1000000000; // 1 frame delay in nanoseconds
    m_SDLWindow = CreateScopedPtr<SDLWindow>();
    m_SDLWindow->Initialize(winProp);

    m_Scene = CreateScopedPtr<Scene>();

    m_Renderer = CreateScopedPtr<SDLNativeRenderer2D>(m_SDLWindow->GetSDLWindow());
    m_Renderer->Initialize();

    auto AppCloseEvent = new Events<void()>("AppClose");
    AppCloseEvent->Subscribe([this]() { m_Running = false; });

    auto AppQuitEvent = new Events<void()>("AppQuit");
    AppQuitEvent->Subscribe([this]() { m_Running = false; });

    auto MouseClickEvent = new Events<void(MouseButtonParam)>("MouseButtonClick");
    MouseClickEvent->Subscribe([](const MouseButtonParam& param)
    {
        std::cout << "Mouse Clicked: " << param.ID << " " << param.X << " " << param.Y << " " << param.Button << " " << param.Clicks << std::endl;
    });



    EventManager::GetInstance().GetEventDispatcher().RegisterEvent(MouseClickEvent);

    EventManager::GetInstance().GetEventDispatcher().RegisterEvent(AppCloseEvent);

    EventManager::GetInstance().GetEventDispatcher().RegisterEvent(AppQuitEvent);

    AssetManager::GetInstance().Initialize(m_Renderer->GetSDLRenderer());

    m_TextEngine = TTF_CreateRendererTextEngine(m_Renderer->GetSDLRenderer());

}

Simulator::~Simulator()
{
    CleanUp();
}

void Simulator::Run()
{
    while(m_Running && m_NumOfAlive > 0)
    {
        auto startTime = SDL_GetTicksNS();
        SDL_Event event;
        HandleEvents(event);
        Update(m_dt);
        Render();
        auto endTime = SDL_GetTicksNS();
        auto deltaTime = endTime - startTime;
        if(deltaTime < m_FrameDelay)
        {
            SDL_DelayNS(m_FrameDelay - deltaTime);
            m_dt = m_FrameDelay / 1000000000.f;
        }
        else
        {
            m_dt = deltaTime / 1000000000.f;
        }
        //std::cout<<1.0f/m_dt<<std::endl;
      
    }
    
    //CleanUp();
}

void Simulator::HandleEvents(SDL_Event& e)
{
    EventManager::GetInstance().OnEvent(e);
}

void Simulator::CleanUp()
{
    if(m_TextEngine)
    {
        //std::cout<<"Destroying Text Engine"<<std::endl;
        TTF_DestroyRendererTextEngine(m_TextEngine);
    }

    
}
#pragma once

#include "Utility.h"
#include "Window.h"
#include "SDLRenderer2D.h"
#include "SDLWindow.h"
#include "Scene.h"
#include <SDL3_ttf/SDL_ttf.h>

class Simulator
{
public:
    virtual ~Simulator();
    virtual void Run();
    virtual void Initialize(const WindowProperties& winProp = WindowProperties{});
    virtual void HandleEvents(SDL_Event& e);
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void CleanUp();
    SDLWindow* GetWindow () { return m_SDLWindow.get(); }
    static TTF_TextEngine* GetTextEngine() { return m_TextEngine; }

protected:
    ScopedPtr<Scene> m_Scene{};
    ScopedPtr<SDLWindow> m_SDLWindow{};
    ScopedPtr<SDLNativeRenderer2D> m_Renderer{};
    inline static TTF_TextEngine* m_TextEngine{};
    bool m_Running = true;
    size_t m_FPS = 60;
    float m_FrameDelay{};
    float m_dt{};
    int m_NumOfAlive{0};
};


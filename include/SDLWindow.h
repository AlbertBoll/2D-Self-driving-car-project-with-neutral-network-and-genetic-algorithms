#pragma once


#include"Window.h"


struct SDL_Window;

class SDLWindow: public Window
{
    
public:
    SDLWindow() = default;
    NONCOPYABLE(SDLWindow);
    virtual ~SDLWindow();
    virtual void Initialize(const WindowProperties& winProp = WindowProperties{}) override;
    virtual void SwapBuffer()override;
    virtual void ShutDown() override;
    virtual void SetTitle(const std::string& title) override;
    SDL_Window* GetSDLWindow()const { return m_Window; }

    // Inherited via Window
    virtual void BeginRender()override;

    virtual void NullRender() override;

    // Inherited via Window
    virtual std::string GetTitle() const override;


    // Inherited via Window
    virtual void OnResize(int new_width, int new_height) override;

private:
    void SetWindow(int DisplayWidth, int DisplayHeight, uint32_t flag, const WindowProperties& winProp = WindowProperties{});
    uint32_t GetWindowFlag(const WindowProperties& winProp);

private:
    SDL_Window* m_Window{ nullptr };
};


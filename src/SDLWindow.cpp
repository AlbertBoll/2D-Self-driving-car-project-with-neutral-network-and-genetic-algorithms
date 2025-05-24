#include "../include/SDLWindow.h"
#include<SDL3/SDL.h>
#include<SDL3_mixer/SDL_mixer.h>
#include<SDL3_ttf/SDL_ttf.h>

SDLWindow::~SDLWindow()
{  
    //std::cout<<"SDLWindow destructor called"<<std::endl;
    ShutDown();
}

void SDLWindow::Initialize(const WindowProperties &winProp)
{
    m_WindowProperties = winProp;
    //initialize SDL3
    if(!SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO))
    {
        std::cerr<<"SDL3_Init Error"<<SDL_GetError()<<std::endl;
        return;
    }

    uint32_t flag = GetWindowFlag(winProp);
    int count{};
    SDL_DisplayID* id = SDL_GetDisplays(&count);
    const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(*id);

    SetWindow(mode->w, mode->h, flag, winProp);

    //Set Window Minimum Size
    SDL_SetWindowMinimumSize(m_Window, winProp.m_MinWidth, winProp.m_MinHeight);


    SDL_SetHintWithPriority(SDL_HINT_MOUSE_EMULATE_WARP_WITH_RELATIVE, "1", SDL_HINT_OVERRIDE);

    //initialize SDL_mixer
    if(!Mix_OpenAudio(0, NULL))
    {
        std::cerr<<"Mix_OpenAudio Error"<<SDL_GetError()<<std::endl;
        return;
    }

    //initialize SDL3 ttf
    if(!TTF_Init())
    {
        std::cerr<<"TTF_Init Error"<<SDL_GetError()<<std::endl;
        return;
    }


}

void SDLWindow::SwapBuffer()
{

}

void SDLWindow::ShutDown()
{
    if(m_Window)
    {
        SDL_DestroyWindow(m_Window);
    }

    //clean Mix
    Mix_CloseAudio();
    Mix_Quit();

    //clean TTF
    TTF_Quit();

    //clean SDL
    SDL_Quit();


}

void SDLWindow::SetTitle(const std::string &title) 
{
    if(m_Window)
    {
        m_WindowProperties.m_Title = title;
        SDL_SetWindowTitle(m_Window,  m_WindowProperties.m_Title.c_str());
    }
}

void SDLWindow::BeginRender()
{

}

void SDLWindow::NullRender()
{

}

std::string SDLWindow::GetTitle() const
{
    return m_WindowProperties.m_Title;
}

void SDLWindow::OnResize(int new_width, int new_height)
{
    m_WindowProperties.m_Width = new_width;
    m_WindowProperties.m_Height = new_height;
}

void SDLWindow::SetWindow(int DisplayWidth, int DisplayHeight, uint32_t flag, const WindowProperties &winProp) 
{
    int topLeftPosX{};
    int topLeftPosY{};

    switch (winProp.m_WinPos)
    {
        case WindowPos::TopLeft:
            topLeftPosX = DisplayWidth / 2 - winProp.m_Width -   winProp.m_XPaddingToCenterY;
            topLeftPosY = DisplayHeight / 2 - winProp.m_Height - winProp.m_YPaddingToCenterX;
            break;

        case WindowPos::TopRight:
            topLeftPosX = DisplayWidth / 2  + winProp.m_XPaddingToCenterY;
            topLeftPosY = DisplayHeight / 2 - winProp.m_Height - winProp.m_YPaddingToCenterX;
            break;

        case WindowPos::ButtomLeft:
            topLeftPosX = DisplayWidth / 2 - winProp.m_Width - winProp.m_XPaddingToCenterY;
            topLeftPosY = DisplayHeight / 2 + winProp.m_YPaddingToCenterX;
            break;

        case WindowPos::ButtomRight:
            topLeftPosX = DisplayWidth / 2  + winProp.m_XPaddingToCenterY;
            topLeftPosY = DisplayHeight / 2 + winProp.m_YPaddingToCenterX;
            break;

        case WindowPos::Center:
            topLeftPosX = DisplayWidth / 2 - winProp.m_Width / 2;
            topLeftPosY = DisplayHeight / 2 - winProp.m_Height / 2;
            break;

    }

    m_Window = SDL_CreateWindow(winProp.m_Title.c_str(), winProp.m_Width, winProp.m_Height, flag);

    if(m_Window)
    {
        bool success = SDL_SetWindowPosition(m_Window, topLeftPosX, topLeftPosY);
        if(!success)
        {
            std::cerr<<"SDL Window failed to set position: "<<SDL_GetError()<<std::endl;
        }
    }

}

uint32_t SDLWindow::GetWindowFlag(const WindowProperties &winProp)
{

    uint32_t flag = 0;

    if (winProp.flag.IsSet(WindowFlags::INVISIBLE))
    {
        flag |= SDL_WINDOW_HIDDEN;
    }

    if (winProp.flag.IsSet(WindowFlags::FULLSCREEN))
    {
        flag |= SDL_WINDOW_FULLSCREEN;
    }

    if (winProp.flag.IsSet(WindowFlags::BORDERLESS))
    {
        flag |= SDL_WINDOW_BORDERLESS;
    }

    if (winProp.flag.IsSet(WindowFlags::RESIZABLE))
    {
        flag |= SDL_WINDOW_RESIZABLE;
    }

    return flag;
}
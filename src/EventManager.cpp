#include "../include/EventManager.h"



void EventManager::Initialize()
{
    

}

void EventManager::OnEvent(SDL_Event &e)
{
    while(SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_EVENT_QUIT:
                m_EventDispatcher.DispatchEvent("AppClose");
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                m_EventDispatcher.DispatchEvent("MouseButtonClick", MouseButtonParam{.ID = e.button.windowID, 
                                                                                     .X = (int)e.button.x, 
                                                                                     .Y = (int)e.button.y, 
                                                                                     .Button = e.button.button, 
                                                                                     .Clicks = e.button.clicks});                                                                    
                break;
            case SDL_EVENT_KEY_DOWN:
                if (e.key.key == SDLK_ESCAPE)
                    m_EventDispatcher.DispatchEvent("AppQuit");  
                else if(e.key.key == SDLK_R)
                    m_EventDispatcher.DispatchEvent("AppResume"); 
                else if(e.key.key == SDLK_P) 
                    m_EventDispatcher.DispatchEvent("AppPause");
                break;
        }
           
    }
}
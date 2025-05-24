#pragma once

#include"base.h"
#include"Event.h"
#include<SDL3/SDL.h>


class EventManager: public Singleton<EventManager>
{
public:
    ~EventManager() = default;
    void Initialize();
    void OnEvent(SDL_Event& e);
    EventDispatcher& GetEventDispatcher() { return m_EventDispatcher; }

private:
    EventDispatcher m_EventDispatcher;

};
#include "../include/Window.h"
#include "../include/SDLWindow.h"
#include "../include/Utility.h"


ScopedPtr<Window> Window::Create(const WindowProperties &winProp)
{
    return CreateScopedPtr<SDLWindow>();
}
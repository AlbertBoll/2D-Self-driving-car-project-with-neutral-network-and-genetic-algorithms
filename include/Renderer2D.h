#pragma once
#include "Math.h"

class Renderer2D
{
public:
    Renderer2D()=default;
    virtual ~Renderer2D() = 0;
    virtual void Initialize() = 0;
    virtual void BeginRender() = 0;
    virtual void Render() = 0;
    virtual void EndRender() = 0;
    Vec4f ReadPixel(int x, int y) = 0;
    

protected:
    int m_WindowWidth{1280};
    int m_WindowHeight{720};
  
};
#include "../include/SDLRenderer2D.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>


void SDLNativeRenderer2D::Initialize() 
{
    if(m_Window)
    {
        m_Renderer = SDL_CreateRenderer(m_Window, NULL);
    }
}

void SDLNativeRenderer2D::RenderRotatedTexture(SDL_Texture *texture, const SDL_FRect *dstrect, const SDL_FRect *srcrect, double angle, const SDL_FPoint *center, SDL_FlipMode flip)
{
    SDL_RenderTextureRotated(m_Renderer, texture, srcrect, dstrect, angle, center, flip);
}

void SDLNativeRenderer2D::RenderLine(const Vec2f &start, const Vec2f &end)
{
    SDL_RenderLine(m_Renderer, start.x, start.y, end.x, end.y);
}

void SDLNativeRenderer2D::RenderText(TTF_Text *text, float x, float y)
{
    TTF_DrawRendererText(text, x, y);
}

void SDLNativeRenderer2D::RenderFillCircle(int centerX, int centerY, int radius)
{
    SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, 255);

    //for(int i = 1; i <= radius; i++)
    //{
        for (int x = 0; x <= radius; x++)
        {
            int y = (int)glm::sqrt(radius * radius - x * x);
            //int y = (int)glm::sqrt(i * i - x * x);
            SDL_RenderPoint(m_Renderer, centerX + x, centerY - y);
            SDL_RenderPoint(m_Renderer, centerX + x, centerY + y);
            SDL_RenderPoint(m_Renderer, centerX - x, centerY - y);
            SDL_RenderPoint(m_Renderer, centerX - x, centerY + y);
            SDL_RenderPoint(m_Renderer, centerX - y, centerY - x);
            SDL_RenderPoint(m_Renderer, centerX - y, centerY + x);
            SDL_RenderPoint(m_Renderer, centerX + y, centerY - x);
            SDL_RenderPoint(m_Renderer, centerX + y, centerY + x);
        }
    //}
}

void SDLNativeRenderer2D::SetDrawColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    SDL_SetRenderDrawColor(m_Renderer, r, g, b, a);
}

void SDLNativeRenderer2D::SetTextureColorMod(SDL_Texture *texture, unsigned char r, unsigned char g, unsigned char b)
{
    SDL_SetTextureColorMod(texture, r, g, b);
}

void SDLNativeRenderer2D::ReadPixels(const SpriteComponent &sprite, std::vector<Color> &pixels)
{
    //SDL_Rect rect = {sprite.m_DesRect.x, sprite.m_DesRect.y, sprite.m_DesRect.w, sprite.m_DesRect.h};
    //SDL_Surface* image_surface = SDL_RenderReadPixels(m_Renderer, &rect);
    SDL_Surface* image_surface = IMG_Load(sprite.m_ImagePath.c_str());
    if(!image_surface)
    {
        std::cerr<<"SDL3_surface Error"<<SDL_GetError()<<std::endl;
        return;
    }

    int w = (int)sprite.m_DesRect.w;
    int h = (int)sprite.m_DesRect.h;
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;

    pixels.resize(w * h);
    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            SDL_ReadSurfacePixel(image_surface, x, y, &r, &g, &b, &a);
            pixels[y * w + x] = {r, g, b, a};
        }
    }

    SDL_DestroySurface(image_surface);
    

}

Color SDLNativeRenderer2D::ReadPixel(const SpriteComponent& sprite, const std::vector<Color> &pixels, int x, int y)
{
    int w = (int)sprite.m_DesRect.w;
    Color pixel_color = pixels[y * w + x];
    //std::cout<<"r: "<<(int)pixel_color.r<<" g: "<<(int)pixel_color.g<<" b: "<<(int)pixel_color.b<<" a: "<<(int)pixel_color.a<<std::endl;
    return pixel_color;
}

void SDLNativeRenderer2D::BeginRender(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    SDL_SetRenderDrawColor(m_Renderer, r, g, b, a);
    SDL_RenderClear(m_Renderer);
}

SDLNativeRenderer2D::~SDLNativeRenderer2D()
{
    //std::cout<<"SDLNativeRenderer2D::~SDLNativeRenderer2D()"<<std::endl;
    if(m_Renderer)
    {
        SDL_DestroyRenderer(m_Renderer);
    }
}

void SDLNativeRenderer2D::EndRender()
{
    SDL_RenderPresent(m_Renderer);
}

void SDLNativeRenderer2D::Render()
{

}

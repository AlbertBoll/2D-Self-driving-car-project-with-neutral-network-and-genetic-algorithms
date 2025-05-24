#pragma once
#include<SDL3/SDL.h>
#include "Math.h"
#include "Component.h"
// struct SDL_Window;
// struct SDL_Renderer;
// struct SDL_Texture;
// struct SDL_FRect;
// struct SDL_FPoint;


class SDLNativeRenderer2D
{
public:
    SDLNativeRenderer2D(SDL_Window* window):m_Window(window){};
    ~SDLNativeRenderer2D();
    void BeginRender(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void EndRender();
    void Render();
    void Initialize();
    SDL_Renderer* GetSDLRenderer()const{return m_Renderer;};
    void RenderRotatedTexture(SDL_Texture* texture, const SDL_FRect *dstrect, const SDL_FRect *srcrect = NULL, double angle = 0, const SDL_FPoint *center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE);
    void RenderLine(const Vec2f& start, const Vec2f& end);
    void RenderText(TTF_Text* text, float x, float y);
    void RenderFillCircle(int centerX, int centerY, int radius);
    void SetDrawColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void SetTextureColorMod(SDL_Texture* texture, unsigned char r, unsigned char g, unsigned char b);
    void ReadPixels(const SpriteComponent& sprite, std::vector<Color>& pixels);
    static Color ReadPixel(const SpriteComponent& sprite, const std::vector<Color>& pixels, int x, int y);


private:
    SDL_Window* m_Window{};
    SDL_Renderer* m_Renderer{};



};
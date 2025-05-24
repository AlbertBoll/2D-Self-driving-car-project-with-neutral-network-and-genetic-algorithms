#pragma once

#include "base.h"
#include <unordered_map>
#include <string>

struct SDL_Texture;
struct Mix_Chunk;
struct TTF_Font;
struct Mix_Music;
struct SDL_Renderer;

class AssetManager: public Singleton<AssetManager>
{
public:
    //AssetManager(SDL_Renderer* renderer);
    ~AssetManager();

    // Get an asset from the manager
    SDL_Texture* GetImage(const std::string& name);
    Mix_Chunk* GetSound(const std::string& name);
    Mix_Music* GetMusic(const std::string& name);
    TTF_Font* GetFont(const std::string& name, int font_size);
    void Initialize(SDL_Renderer* renderer);

private:
    void CleanUp();
    // Load an asset into the manager
    void LoadImage(const std::string& file_path);
    void LoadSound(const std::string& file_path);
    void LoadMusic(const std::string& file_path);
    void LoadFont(const std::string& file_path, int font_size);

private:
    SDL_Renderer* m_Renderer{ nullptr};
    std::unordered_map<std::string, SDL_Texture*> m_Textures;
    std::unordered_map<std::string, Mix_Chunk*> m_Sounds;
    std::unordered_map<std::string, TTF_Font*> m_Fonts;
    std::unordered_map<std::string, Mix_Music*> m_Musics;

};
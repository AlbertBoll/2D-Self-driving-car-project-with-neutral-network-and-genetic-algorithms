#include "../include/AssetManager.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3/SDL.h>
#include <iostream>


void AssetManager::LoadImage(const std::string &file_path)
{
    SDL_Texture* texture = IMG_LoadTexture(m_Renderer, file_path.c_str());

    if (texture == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load image: %s", file_path.c_str());
        return;
    }
    m_Textures.emplace(file_path, texture);

}

void AssetManager::LoadSound(const std::string &file_path)
{
    Mix_Chunk* sound = Mix_LoadWAV(file_path.c_str());

    if (sound == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load sound: %s", file_path.c_str());
        return;
    }
    m_Sounds.emplace(file_path, sound);
}

void AssetManager::LoadMusic(const std::string &file_path)
{
    Mix_Music* music = Mix_LoadMUS(file_path.c_str());

    if (music == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s", file_path.c_str());
        return;
    }
    m_Musics.emplace(file_path, music);
}

void AssetManager::LoadFont(const std::string &file_path, int font_size)
{
    TTF_Font* font = TTF_OpenFont(file_path.c_str(), font_size);

    if (font == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load font: %s", file_path.c_str());
        return;
    }
    m_Fonts.emplace(file_path + std::to_string(font_size), font);
}



AssetManager::~AssetManager()
{
    //std::cout<<"AssetManager::~AssetManager()"<<std::endl;
    for(auto &texture : m_Textures)
    {
        SDL_DestroyTexture(texture.second);
    }
    m_Textures.clear();

    for(auto &sound : m_Sounds)
    {
        Mix_FreeChunk(sound.second);
    }
    m_Sounds.clear();

    for(auto &music : m_Musics)
    {
        Mix_FreeMusic(music.second);
    }
    m_Musics.clear();

    for(auto &font : m_Fonts)
    {
        TTF_CloseFont(font.second);
    }
    m_Fonts.clear();
}

SDL_Texture *AssetManager::GetImage(const std::string &path)
{
    if(auto it = m_Textures.find(path); it != m_Textures.end())
    {
        return it->second;
    }
    LoadImage(path);
    return m_Textures[path];
}

Mix_Chunk *AssetManager::GetSound(const std::string &name)
{
    if(auto it = m_Sounds.find(name); it != m_Sounds.end())
    {
        return it->second;
    }
    LoadSound(name);
    return m_Sounds[name];
}

Mix_Music *AssetManager::GetMusic(const std::string &name)
{
    if(auto it = m_Musics.find(name); it != m_Musics.end())
    {
        return it->second;
    }
    LoadMusic(name);
    return m_Musics[name];
}

TTF_Font *AssetManager::GetFont(const std::string &file, int font_size)
{
    if(auto it = m_Fonts.find(file + std::to_string(font_size)); it != m_Fonts.end())
    {
        return it->second;
    }
    LoadFont(file, font_size);
    return m_Fonts[file + std::to_string(font_size)];
}

void AssetManager::Initialize(SDL_Renderer *renderer)
{
    m_Renderer = renderer;
}

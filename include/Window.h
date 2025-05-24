#pragma once

#include <cstdint>
#include <string>
#include "utility.h"

enum class WindowFlags: uint8_t
{
	NONE = 0x0,
	INVISIBLE = 0x1,
	FULLSCREEN = 0X2,
	BORDERLESS = 0x4,
	RESIZABLE = 0x8
};


enum class WindowPos: uint8_t
{
	TopLeft,
	TopRight,
	ButtomLeft,
	ButtomRight,
	Center

};

struct WindowProperties
{
	std::string m_Title = "Genetic Algorithm Simulator";
	uint32_t m_Width = 960;
	uint32_t m_Height = 540;
	uint32_t m_MinWidth = 800;
	uint32_t m_MinHeight = 540;

	float m_AspectRatio = 16.f / 9.f;
	float m_Red = 0.f;
	float m_Green = 0.f;
	float m_Blue = 0.f;
	BitFlags<WindowFlags, uint8_t>flag{ WindowFlags::RESIZABLE };
	WindowPos m_WinPos = WindowPos::Center;
	int m_XPaddingToCenterY = 20;
	int m_YPaddingToCenterX = 20;
	bool m_IsVsync = true;

	void SetColor(float R, float G, float B) { m_Red = R; m_Green = G; m_Blue = B; }
	void SetCornFlowerBlue() {
		m_Red = static_cast<float>(0x64)/ static_cast<float>(0xFF); 
		m_Green = static_cast<float>(0x95)/ static_cast<float>(0xFF);
		m_Blue = static_cast<float>(0xED)/ static_cast<float>(0xFF);
	}
};

class Window
{
    public:

    virtual ~Window(){};
    uint32_t GetScreenWidth()const { return m_WindowProperties.m_Width; }
    uint32_t GetScreenHeight()const { return m_WindowProperties.m_Height; }
    virtual void Initialize(const WindowProperties& winProp = {}) = 0;
    virtual void SwapBuffer() = 0;
    virtual void ShutDown() = 0;
    virtual void SetTitle(const std::string& title) = 0;
    virtual std::string GetTitle() const = 0;
    //virtual uint32_t GetWindowID()const = 0;

    virtual void NullRender() = 0;
    virtual void BeginRender()  = 0;
    //virtual void EndRender(BaseApp* app)  = 0;
    virtual void OnResize(int new_width, int new_height) = 0;
	WindowProperties& GetWindowProperties(){ return m_WindowProperties; }

    static ScopedPtr<Window> Create(const WindowProperties& winProp = {});

protected:
	WindowProperties m_WindowProperties;
    //uint32_t m_ScreenWidth, m_ScreenHeight;
    //float m_AspectRatio = 16.f / 9.f;
};
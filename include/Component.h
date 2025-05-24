#pragma once

#include "UUID.h"
#include <SDL3/SDL.h>
#include <string>
#include "Math.h"
#include "Utility.h"
#include "Math.h"
#include "Signal.h"


struct IDComponent
{
	UUID ID;
	IDComponent() = default;
	IDComponent(UUID id): ID(id){}
	IDComponent(const IDComponent&) = default;
	operator UUID() const { return ID; }
};


struct TagComponent
{
	std::string Name;
	TagComponent() = default;
	TagComponent(const std::string& name) : Name(name) {}
	operator std::string() { return Name; }
	operator std::string() const { return Name; }
};

//struct SDL_Texture;
struct SpriteComponent
{
    SDL_Texture* m_Texture;
    SDL_FRect m_SrcRect = {0, 0, 0, 0};
    SDL_FRect m_DesRect = {0, 0, 0, 0};
    float m_Angle = 0.f;
    bool m_Flip = false;
	Vec2f m_SpriteSize = {1.f, 1.f};
	Vec2f m_Scale = {1.f, 1.f};
	std::string m_ImagePath{};
    SpriteComponent() = default;
    SpriteComponent(const std::string& file_path);
    void SetSrcRect(const SDL_FRect& rect){ m_SrcRect = rect; };
    void SetDesRect(const SDL_FRect& rect){ m_DesRect = rect; }
	void SetScale(float scale_x, float scale_y);
};

struct SpriteComponents
{
    std::vector<SpriteComponent> m_Sprites;
    SpriteComponents(const std::vector<SpriteComponent>& sprites) : m_Sprites(sprites) {};
	void AddSprite(const SpriteComponent& sprite){ m_Sprites.push_back(sprite); }

};

struct Transform2DComponent
{
    Vec2f m_Translation{ 0.0f, 0.0f };
    Vec2f m_Scale{ 1.0f, 1.0f };
    float m_Rotation{ 0.f };
	Transform2DComponent() = default;
    Transform2DComponent(const Vec2f& translation, const Vec2f& scale, float rotation): m_Translation(translation), m_Scale(scale), m_Rotation(rotation) {}
	//Transform2DComponent(const Transform2DComponent& other) = default;
};

struct RelationshipComponent
{
	UUID ParentHandle = 0;
	std::vector<UUID> Children;

	RelationshipComponent() = default;
	RelationshipComponent(const RelationshipComponent& other) = default;
	RelationshipComponent(UUID parent): ParentHandle(parent) {}

};

struct RelationComponent
{

};

struct TextComponent;

struct Movement2DComponent
{
    //Vec2f m_Velocity{ 0.0f, 0.0f };
	float m_MaxSpeed{70.f};
	float m_Acceleration{0.f};
	float m_Speed{0.f};
	float m_SlippingSpeed{0.f};
	//float m_Drag{0.f};
	bool m_IsShutOff{false};
	int m_LastCheckPointPassed{0};
	Movement2DComponent(){m_SlippingSpeed = m_MaxSpeed * 0.75f; }
	void SetSpeed(float speed);
	void Reset();
	float GetSpeed() const {return m_Speed;}
	void HitCheckPoint(int checkPoint);
	Signal<void(float)> m_OnSpeedChanged;

};

struct InputComponent
{
    const bool* m_CurrentKeyStates{};
	InputComponent& Update()
	{
		m_CurrentKeyStates = SDL_GetKeyboardState(NULL);
		return *this;
	}

};

struct TTF_Text;

struct TextComponent
{
    TTF_Text* m_Text{ nullptr};
	std::string m_FontPath{""};
	std::string m_TextString{""};
    int m_FontSize{ 16 };
	float m_X{};
	float m_Y{};
	int m_Width{};
	int m_Height{};
	Color m_Color{ 0, 0, 0, 255};

	TextComponent(const std::string& font_path, const std::string& text, int font_size);
	~TextComponent();
	void SetText(const std::string& text, const Color& color = {0, 0, 0, 255});
	void SetColor(const Color& color);
	void SetFont(const std::string& font_path, int font_size);
	void SetTextString(const std::string& text);
	void SetFontSize(int font_size);
	void UpdateSize();
	TextComponent& SetPosition(float x, float y);

	void HandleSpeedChange(float new_speed);
	void HandlePopulationChange(int new_population);
	void HandleRoundChange(int new_round);

};

struct ControlParameter
{
    float m_Acceleration{ 0.0f };
    float m_SteerPosition{ 0.0f };
};

struct FirstAINeuralNetworkComponent
{
    FirstAINeuralNetworkComponent& FeedForward();
	ControlParameter m_ControlParameter{};
};


struct Layer
{
	Layer(const std::vector<float>& weights): m_Weights(weights){}
	Layer(int input_size, int output_size);
	void FeedForward(const std::vector<float>& inputs);

	std::vector<float> m_Outputs{};
	std::vector<float> m_Weights{};
};

struct RankableChromosome
{
    RankableChromosome(int highest_checkpoint, const std::vector<float>& chromosome):m_HighestCheckpoint(highest_checkpoint), m_Chromosome(chromosome){}
    int m_HighestCheckpoint{};
    std::vector<float> m_Chromosome{};
	bool operator > (const RankableChromosome& other) const
	{
		return m_HighestCheckpoint > other.m_HighestCheckpoint;
	}
};

struct Evolution
{
	Evolution() = default;
    Evolution(int population_count, int keep_count):m_PopulationCount(population_count), m_KeepCount(keep_count){};
    std::vector<std::vector<float>> Evolve(const std::vector<RankableChromosome>& chromosomes);
	Evolution& Selection();
	Evolution& Crossover();
	Evolution& Mutation();

	int m_PopulationCount{};
	int m_KeepCount{};
};

struct AINeuralNetworkComponent
{
    AINeuralNetworkComponent() = default;
    AINeuralNetworkComponent(const std::vector<int>& dimensions);
	AINeuralNetworkComponent(const std::vector<int>& dimensions, const std::vector<Layer>& layers);
	std::vector<float> FeedForward(const std::vector<float>& inputs);
	RankableChromosome Serialize();
	void Deserialize(const std::vector<float>& chromosome);

	std::vector<int> m_Dimensions{};
	std::vector<float> m_Inputs{};
	std::vector<Layer> m_Layers{};
	int m_HighestCheckpoint{0};
};

struct RadarComponent
{
    Vec2f m_Start{};
	Vec2f m_End{};
	float m_Angle{0.0f};
	float m_MaxDistance{200.0f};
	RadarComponent(float angle);
	Vec2f CalculateEnd(const Vec2f& start, float angle, float distance);
	float Probe(const SpriteComponent &track_sprite, const Transform2DComponent& transform);
};

struct RadarComponents
{
	RadarComponents();
	std::vector<RadarComponent> m_RadarComponents{};
	std::vector<float> Probe(const SpriteComponent &track_sprite, const Transform2DComponent& transform);
	bool IsShowRadars{true};

};

struct NeuronHUDDisplayComponent
{
    NeuronHUDDisplayComponent(const SpriteComponent& sprite_component): m_NeuronSprite(sprite_component) {}
	void FetchNeuronOutputsInEachLayer(const AINeuralNetworkComponent& ai_neural_network);
	NeuronHUDDisplayComponent& SetNeuronsPosition(const std::vector<Vec2f>& neurons_position);
	std::vector<float> m_NeuronValues{};
	std::vector<Vec2f> m_NeuronsPosition{};
	SpriteComponent m_NeuronSprite;
};

struct CheckPointsComponent
{
	CheckPointsComponent(const std::vector<Vec2f>& check_points, const SpriteComponent& sprite);
	std::vector<Vec2f> m_CheckPoints{};
	SpriteComponent m_Sprite;
	
	int m_CurrentCheckPoint{0};
	float m_CheckPointRadius{25.0f};
	bool IsPassedCheckPoint(const Vec2f& position);
};

template<typename... Components>
requires(Types<Components...>::Is_All_Different_Type)
struct ComponentsGroup
{

};

using AllComponents = ComponentsGroup<Movement2DComponent, Transform2DComponent, SpriteComponent, RelationshipComponent, InputComponent>;
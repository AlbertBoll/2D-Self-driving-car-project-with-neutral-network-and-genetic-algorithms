#pragma once
#include"include/base.h"
#include"include/Simulator.h"
#include"include/Entity.h"
#include<json.hpp>

using json = nlohmann::json;

class GeneticAlgorithmSimulator: public Simulator, public Singleton<GeneticAlgorithmSimulator>
{
public:
    ~GeneticAlgorithmSimulator();
    void Run()override;
    void Initialize(const WindowProperties& winProp = WindowProperties{})override;
    void Update(float dt)override;
    void Render()override;
    void HandleEvents(SDL_Event& e)override;
    void CleanUp()override;

    bool IsRoad(const SpriteComponent &track_sprite, std::vector<Color> &pixels, int x, int y);
    bool IsRoad(const SpriteComponent &track_sprite, int x, int y);
    bool IsRoad(const SpriteComponent &track_sprite, const Vec2f& coord);
    void FillCars(int number);
    void ResetCars();
    void FillHud();
    GeneticAlgorithmSimulator& SetTotalSimulationRound(int rounds)
    {
        m_TotalSimulationRound = rounds;
        return *this;
    }
    

    template<typename component1, typename component2>
    void ConnectTwoEntities(Entity& e1, Entity& e2)
    {

    };

    template<>
    void ConnectTwoEntities<Movement2DComponent, TextComponent>(Entity& e1, Entity& e2)
    {
        auto& movement = e1.GetComponent<Movement2DComponent>();
        auto& text = e2.GetComponent<TextComponent>();
        text.SetText("Speed: " + DecimalFormatTo<2>(movement.m_Speed));
    }


    Entity GetTrack(){return m_Track;}
    int GetTotalCarsPopulation(){return (int)m_Cars.size();}
    void SetAlivePopulation(int population);
    int CountAlivePopulation();


private:
    Entity m_Track;
    Entity m_TrackOverlay;
    //Entity m_Car;
    std::vector<Color> m_Pixels;
    std::vector<Entity> m_Cars;
    std::vector<Entity> m_Hud;
    //json m_CheckPoints;
    std::vector<Vec2f> m_CheckPoints;
    Signal<void(int)> m_OnAlivePopulationChange;
    Signal<void(int)> m_OnRoundChange;
    int m_TotalSimulationRound{1};
    Evolution m_Evolution;
    
};
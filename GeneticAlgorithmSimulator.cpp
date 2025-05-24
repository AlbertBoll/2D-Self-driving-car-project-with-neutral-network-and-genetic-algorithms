#define GLM_ENABLE_EXPERIMENTAL
#include"GeneticAlgorithmSimulator.h"
#include<SDL3/SDL.h>
#include"include/Entity.h"
#include"include/Random.h"
#include <fstream>
#include <glm/gtx/norm.hpp>

GeneticAlgorithmSimulator::~GeneticAlgorithmSimulator()
{
    //Simulator::~Simulator();
}

void GeneticAlgorithmSimulator::Run()
{       
        m_Evolution = Evolution(40, 4);
        //std::vector<AINeuralNetworkComponent> neural_networks;
        //neural_networks.reserve(m_Cars.size());
        int simulation_round = 1;
        std::vector<RankableChromosome>serialized;
        serialized.reserve(m_Cars.size());

        while(simulation_round <= m_TotalSimulationRound)
        {
            m_OnRoundChange(simulation_round);

            Simulator::Run();
            int total_network = 0;
            int sum = 0;
            //after each run, collect highest checkpoint each car reached
            for(auto& ent: m_Scene->GetAllEntitiesWith<Movement2DComponent, AINeuralNetworkComponent>())
            //for(auto& entity: m_Cars)
            {
                Entity entity = {ent, m_Scene.get()};
                auto& neural_network = entity.GetComponent<AINeuralNetworkComponent>();
                auto& movement = entity.GetComponent<Movement2DComponent>();
                neural_network.m_HighestCheckpoint = movement.m_LastCheckPointPassed;
                //neural_networks.emplace_back(neural_network.m_Dimensions, neural_network.m_Layers);
                serialized.push_back(neural_network.Serialize());
                
                sum += neural_network.m_HighestCheckpoint;
                total_network++;
            }
            std::cout<<"Entity Size: "<<m_Scene->GetEntityCount()<<std::endl;
            std::cout<<"=== Round: "<<simulation_round<<" ==="<<std::endl;
            std::cout<<"-- Average checkpoint reached: "<<(float)sum/(float)total_network<<std::endl;
            ResetCars();
            auto chromosomes = m_Evolution.Evolve(serialized);

            //for(auto i = 0; i < m_Cars.size(); i++)
            int i = 0;
            for(auto& ent: m_Scene->GetAllEntitiesWith<AINeuralNetworkComponent>())
            {
                Entity entity = {ent, m_Scene.get()};
                entity.GetComponent<AINeuralNetworkComponent>().Deserialize(chromosomes[i++]);
                //m_Cars[i].GetComponent<AINeuralNetworkComponent>().Deserialize(chromosomes[i]);               
            }

            //FillHud();
            simulation_round++;
            serialized.clear();
        }
     
}

void GeneticAlgorithmSimulator::Initialize(const WindowProperties &winProp)
{
    Random::Init();
    Simulator::Initialize(winProp);

    std::ifstream f("checkpoints/track1.json");
    json CheckPoints = json::parse(f);
    for(auto& checkpoint : CheckPoints["checkpoints"])
    {
        m_CheckPoints.push_back({checkpoint[0], checkpoint[1]});
    }

    // configure track 
    SpriteComponent track_sprite("assets/images/track1_1280_720.png");

    // configure circle for checkpoints and neurons
    SpriteComponent circle_sprite("assets/images/circle_update.png");
    SpriteComponent neuron_sprite("assets/images/circle_update.png");
    
    
    circle_sprite.SetScale(50.0f / circle_sprite.m_SpriteSize.x, 50.0f / circle_sprite.m_SpriteSize.y);
    neuron_sprite.SetScale(70.0f / neuron_sprite.m_SpriteSize.x, 70.0f / neuron_sprite.m_SpriteSize.y);
    //circle_sprite.SetOrigin(0.5f, 0.5f);
   
    track_sprite.m_DesRect.w = (float)winProp.m_Width;
    track_sprite.m_DesRect.h = (float)winProp.m_Height;

    m_Track = m_Scene->CreateEntity("track sprite");
    m_Renderer->ReadPixels(track_sprite, m_Pixels);

    std::string font_path = "assets/fonts/Kormulator-T3.ttf";
    auto& text = m_Track.AddOrReplaceComponent<TextComponent>(font_path, "", 32);
    text.UpdateSize();

    std::vector<int> dimensions{5, 4, 2};
    std::vector<Vec2f>neurons_position;
    neurons_position.reserve(11);
    int x = 45;
    for(auto neuron_count: dimensions)
    {
        auto total_height = neuron_count * 70 -10;
        int y = (winProp.m_Height - total_height) / 2;
        for(int i = 0; i < neuron_count; ++i)
        {
            neurons_position.emplace_back(x, y);
            y += 70;
        }
        x += 70;
    }

    m_Track.AddComponent<SpriteComponent>(track_sprite);
    m_Track.AddComponent<CheckPointsComponent>(m_CheckPoints, circle_sprite);
    m_Track.AddComponent<NeuronHUDDisplayComponent>(neuron_sprite).SetNeuronsPosition(neurons_position);

    //configure track overlay
    SpriteComponent track_overlay_sprite("assets/images/track1-overlay_1280_720.png");
    track_overlay_sprite.m_DesRect.w = (float)winProp.m_Width;
    track_overlay_sprite.m_DesRect.h = (float)winProp.m_Height;
    m_TrackOverlay = m_Scene->CreateEntity("track_overlay sprite");
    m_TrackOverlay.AddComponent<SpriteComponent>(track_overlay_sprite);

    //configure checkpoints



    //configure car
    FillCars(40);

    FillHud();
    
}

void GeneticAlgorithmSimulator::Update(float dt)
{
    auto& track_sprite = m_Track.GetComponent<SpriteComponent>();
    auto& checkpoints = m_Track.GetComponent<CheckPointsComponent>().m_CheckPoints;
    auto& neurons_hud = m_Track.GetComponent<NeuronHUDDisplayComponent>();

    for(auto& ent: m_Scene->GetAllEntitiesWith<SpriteComponent, Movement2DComponent, Transform2DComponent, AINeuralNetworkComponent, RadarComponents>())
    //for(auto& entity: m_Cars)
    {
        Entity entity = {ent, m_Scene.get()};
        auto& transform = entity.GetComponent<Transform2DComponent>();
        auto& sprite = entity.GetComponent<SpriteComponent>();
        auto& movement = entity.GetComponent<Movement2DComponent>();
        auto& radars = entity.GetComponent<RadarComponents>();
        auto measurements = radars.Probe(track_sprite, transform);
        auto& neural_network = entity.GetComponent<AINeuralNetworkComponent>();
        auto outputs = neural_network.FeedForward(measurements);

        movement.m_Speed -= 0.5f; //friction

        float steer_impact{};

        if(!IsRoad(track_sprite, transform.m_Translation))
        {
            movement.m_IsShutOff = true;
            radars.IsShowRadars = false;
            //continue;
        }

        for(auto i = 0; i < checkpoints.size(); i++)
        {
            if(glm::length2(checkpoints[i]-transform.m_Translation) < 2000.0f)
            {
                movement.HitCheckPoint(i);
            }
        }
    
        if(!movement.m_IsShutOff)
        {
            float acceleration = outputs[0];
            float steer_position = outputs[1];
            float slipping_speed = movement.m_SlippingSpeed;

            if (acceleration > 0)
            {
                movement.SetSpeed(movement.GetSpeed() + 1.f);
            }

            if(movement.m_Speed > slipping_speed)
            {
                steer_impact = -movement.m_Speed / movement.m_MaxSpeed + slipping_speed / movement.m_MaxSpeed + 1;
            }
            else
            {
                steer_impact = 1;
            }
            transform.m_Rotation -= steer_position * steer_impact * movement.m_Speed * dt * 10;
        }
        else
        {
            //movement.m_Speed = movement.GetSpeed() - 0.05f * movement.GetSpeed();
            movement.SetSpeed(movement.GetSpeed() - 0.05f * movement.GetSpeed());
        }

        movement.SetSpeed(glm::clamp(movement.m_Speed, 0.0f, movement.m_MaxSpeed));


        if(movement.m_Speed <= 0.0f)
        {
            radars.IsShowRadars = false;
            movement.m_IsShutOff = true;  
        }


        transform.m_Translation.x += movement.m_Speed * glm::cos(glm::radians(transform.m_Rotation)) * dt * 5;
        transform.m_Translation.y -= movement.m_Speed * glm::sin(glm::radians(transform.m_Rotation)) * dt * 5;
        sprite.m_Angle = -transform.m_Rotation;
        sprite.m_DesRect.x = transform.m_Translation.x - sprite.m_SpriteSize.x / 2;
        sprite.m_DesRect.y = transform.m_Translation.y - sprite.m_SpriteSize.y / 2;

        //entity.Update(dt);
    }

    //update the alive populations
    SetAlivePopulation(CountAlivePopulation());
    neurons_hud.FetchNeuronOutputsInEachLayer(m_Cars[0].GetComponent<AINeuralNetworkComponent>());
}

void GeneticAlgorithmSimulator::Render()
{
    m_Renderer->BeginRender(0, 0, 0, 255);
    auto& parking_lot_sprite = m_Track.GetComponent<SpriteComponent>();
    auto& track_overlay_sprite = m_TrackOverlay.GetComponent<SpriteComponent>();
    auto& checkpoints = m_Track.GetComponent<CheckPointsComponent>();
    auto& check_point_text = m_Track.GetComponent<TextComponent>();
    check_point_text.SetFontSize(32);
    auto& check_point_sprite = checkpoints.m_Sprite;

    //auto& car_sprite = m_Car.GetComponent<SpriteComponent>();

    m_Renderer->RenderRotatedTexture(parking_lot_sprite.m_Texture, &parking_lot_sprite.m_DesRect, &parking_lot_sprite.m_SrcRect, parking_lot_sprite.m_Angle);

    //render check points
    int i = 0;
    check_point_text.SetColor({0, 0, 0, 255});
    for(auto& checkpoint: checkpoints.m_CheckPoints)
    {
        check_point_sprite.m_DesRect.x = checkpoint.x - check_point_sprite.m_SpriteSize.x / 2;
        check_point_sprite.m_DesRect.y = checkpoint.y - check_point_sprite.m_SpriteSize.y / 2;
        check_point_text.SetText(std::to_string(i));
        check_point_text.SetPosition(checkpoint.x - 9, checkpoint.y - 17); //- check_point_text.m_Width / 2., checkpoint.y - check_point_text.m_Height / 2.);
        m_Renderer->RenderRotatedTexture(check_point_sprite.m_Texture, &check_point_sprite.m_DesRect, &check_point_sprite.m_SrcRect);
        m_Renderer->RenderText(check_point_text.m_Text, check_point_text.m_X, check_point_text.m_Y);
        i++;
        //m_Renderer->RenderFillCircle(checkpoint.x, checkpoint.y, checkpoints.m_CheckPointRadius);
    }

    for(auto car: m_Cars)
    {
        auto& radars = car.GetComponent<RadarComponents>();
        auto& radars_comps = radars.m_RadarComponents;
        bool is_show_radar = radars.IsShowRadars;
        for(auto& radar: radars_comps)
        {
            //draw radar line
            if(is_show_radar)
                m_Renderer->RenderLine(radar.m_Start, radar.m_End);
        }
        auto& car_sprite = car.GetComponent<SpriteComponent>();
        m_Renderer->RenderRotatedTexture(car_sprite.m_Texture, &car_sprite.m_DesRect, &car_sprite.m_SrcRect, car_sprite.m_Angle);
    }


    //draw overlay
    m_Renderer->RenderRotatedTexture(track_overlay_sprite.m_Texture, &track_overlay_sprite.m_DesRect, &track_overlay_sprite.m_SrcRect, track_overlay_sprite.m_Angle);

    //Draw HUD
    for(auto hud: m_Hud)
    {
        auto& text_component = hud.GetComponent<TextComponent>();
        m_Renderer->RenderText(text_component.m_Text, text_component.m_X, text_component.m_Y);
    }

    //draw neurons hud
    auto& neurons_hud = m_Track.GetComponent<NeuronHUDDisplayComponent>();
    auto neuron_sprite = neurons_hud.m_NeuronSprite;
    auto values = neurons_hud.m_NeuronValues;
    
    int index = 0;
    for(auto& pos: neurons_hud.m_NeuronsPosition)
    {   
        float value = values[index++];
        if(value > 0.f)
        {
            m_Renderer->SetTextureColorMod(neuron_sprite.m_Texture, 0, (int)(value * 200.f), 0);
        }
        else
        {
            m_Renderer->SetTextureColorMod(neuron_sprite.m_Texture, (int)(value * 200.f), 0, 0);
        }
        check_point_text.SetText(DecimalFormatTo<2>(value));
        check_point_text.SetPosition(pos.x - 19, pos.y - 13);
        check_point_text.SetFontSize(20);
        neuron_sprite.m_DesRect.x = pos.x - neuron_sprite.m_SpriteSize.x/2;
        neuron_sprite.m_DesRect.y = pos.y - neuron_sprite.m_SpriteSize.y/2;
        m_Renderer->RenderRotatedTexture(neuron_sprite.m_Texture, &neuron_sprite.m_DesRect, &neuron_sprite.m_SrcRect, neuron_sprite.m_Angle);
      
        check_point_text.SetColor({255, 255, 255, 255});
        m_Renderer->RenderText(check_point_text.m_Text, check_point_text.m_X, check_point_text.m_Y);
       
    }
    m_Renderer->SetTextureColorMod(neuron_sprite.m_Texture, 255, 255, 255);




    // for(auto& car : m_Scene->GetAllEntitiesWith<SpriteComponent, FirstAINeuralNetworkComponent>())
    // {
    //     Entity car_entity = {car, m_Scene.get()};
    //     auto& car_sprite = car_entity.GetComponent<SpriteComponent>(); 
    //     m_Renderer->RenderRotatedTexture(car_sprite.m_Texture, &car_sprite.m_DesRect, &car_sprite.m_SrcRect, car_sprite.m_Angle);
    // }
    
    m_Renderer->EndRender();
}

void GeneticAlgorithmSimulator::HandleEvents(SDL_Event &e)
{
    Simulator::HandleEvents(e);
}

void GeneticAlgorithmSimulator::CleanUp()
{
    Simulator::CleanUp();
}

bool GeneticAlgorithmSimulator::IsRoad(const SpriteComponent &track_sprite, std::vector<Color> &pixels, int x, int y)
{
    pixels.clear();
    m_Renderer->ReadPixels(track_sprite, pixels);
    int w = (int)track_sprite.m_DesRect.w;

    Color color = pixels[x + y * w];
    if (color.r == 75 && color.g == 75 && color.b == 75 && color.a == 255)
    {
        return true;
    }

    return false;
}

bool GeneticAlgorithmSimulator::IsRoad(const SpriteComponent &track_sprite, int x, int y)
{
    int w = (int)track_sprite.m_DesRect.w;

    Color color = m_Pixels[x + y * w];
    if (color.r == 75 && color.g == 75 && color.b == 75 && color.a == 255)
    {
        return true;
    }

    return false;
}

bool GeneticAlgorithmSimulator::IsRoad(const SpriteComponent &track_sprite, const Vec2f &coord)
{
    if(int(coord.x) < 0 || int(coord.x) >= (int)track_sprite.m_DesRect.w || int(coord.y) < 0 || int(coord.y) >= (int)track_sprite.m_DesRect.h)
        return false;
    return IsRoad(track_sprite, (int)coord.x, (int)coord.y);
}

void GeneticAlgorithmSimulator::FillCars(int number)
{
    m_Cars.clear();
    m_Cars.reserve(number);
    m_NumOfAlive = number;
    int num{};
    for (int i = 0; i < m_NumOfAlive; i++)
    {
        num = Random::UInt(0, 4);
        std::string path = "assets/images/car" + std::to_string(num) + ".png";
        Entity entity = m_Scene->CreateEntity("car_"+std::to_string(i));
        auto& sprite = entity.AddOrReplaceComponent<SpriteComponent>(path);
        sprite.SetScale(1.5f, 1.5f);
        auto& transform = entity.AddOrReplaceComponent<Transform2DComponent>();
        transform.m_Translation = {m_CheckPoints[0].x, m_CheckPoints[0].y};
        //transform.m_Translation = { (float)m_SDLWindow->GetWindowProperties().m_Width / 2, (float)m_SDLWindow->GetWindowProperties().m_Height / 2 };
        //std::cout<<"transform.m_Translation: "<<transform.m_Translation.x<<" "<<transform.m_Translation.y<<std::endl;
        sprite.m_DesRect.x = transform.m_Translation.x - sprite.m_SpriteSize.x / 2.f;
        sprite.m_DesRect.y = transform.m_Translation.y - sprite.m_SpriteSize.y / 2.f;
        entity.AddOrReplaceComponent<Movement2DComponent>();
        entity.AddOrReplaceComponent<AINeuralNetworkComponent>(std::vector{5, 4, 2});
        entity.AddOrReplaceComponent<RadarComponents>();
        m_Cars.push_back(entity);
    }
}

void GeneticAlgorithmSimulator::ResetCars()
{
    //for (auto& entity : m_Cars)
    for(auto& ent: m_Scene->GetAllEntitiesWith<Movement2DComponent, AINeuralNetworkComponent>())
    {
        Entity entity = {ent, m_Scene.get()};
        entity.GetComponent<Movement2DComponent>().Reset();
        entity.AddOrReplaceComponent<AINeuralNetworkComponent>(std::vector{5, 4, 2});
        entity.AddOrReplaceComponent<RadarComponents>();
        entity.AddOrReplaceComponent<Transform2DComponent>(Vec2f{m_CheckPoints[0].x, m_CheckPoints[0].y}, Vec2f{1.f, 1.f}, 0.f);
    }
    m_NumOfAlive = (int)m_Cars.size();
    
}

void GeneticAlgorithmSimulator::FillHud()
{

    m_OnAlivePopulationChange.DisconnectAll();
    m_OnRoundChange.DisconnectAll();

    m_Hud.clear();
    m_Hud.reserve(3);
    std::string font_path = "assets/fonts/Kormulator-T3.ttf";
    Entity entity1 = m_Scene->CreateEntity("hud_round");
    auto& round_text = entity1.AddOrReplaceComponent<TextComponent>(font_path, "Round: 1", 32).SetPosition(30, 10);
    Connection(*this, m_OnRoundChange, round_text, &TextComponent::HandleRoundChange);
    m_Hud.push_back(entity1);

    Entity entity2 = m_Scene->CreateEntity("hud_population");
    auto& population_text = entity2.AddOrReplaceComponent<TextComponent>(font_path, "Population: " + std::to_string(m_Cars.size()) + "/" + std::to_string(m_Cars.size()), 32).SetPosition(240, 10);
    Connection(*this, m_OnAlivePopulationChange, population_text, &TextComponent::HandlePopulationChange);
    m_Hud.push_back(entity2);


    Entity entity3 = m_Scene->CreateEntity("hud_speed");
    auto& speed_text = entity3.AddOrReplaceComponent<TextComponent>(font_path, "Speed: " + DecimalFormatTo<2>(m_Cars[0].GetComponent<Movement2DComponent>().m_Speed), 32).SetPosition(640, 10);
    auto Entity_1 = m_Cars[0];
    auto& movement = Entity_1.GetComponent<Movement2DComponent>();
    Connection(movement, m_OnSpeedChanged, speed_text, &TextComponent::HandleSpeedChange);

    m_Hud.push_back(entity3);


}

void GeneticAlgorithmSimulator::SetAlivePopulation(int alive_population)
{
    if(alive_population != m_NumOfAlive)
    {
        m_OnAlivePopulationChange(alive_population); 
        m_NumOfAlive = alive_population;
        
    }
}

int GeneticAlgorithmSimulator::CountAlivePopulation()
{
   int count = 0;
   for(auto& car : m_Cars)
   {
       if(!car.GetComponent<Movement2DComponent>().m_IsShutOff)
       {
           count++;
       }
   }
   return count;
}

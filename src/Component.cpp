#include "../include/Component.h"
#include "../include/AssetManager.h"
#include "../include/Random.h"
#include <SDL3_ttf/SDL_ttf.h>
#include "../include/Simulator.h"
#include "../GeneticAlgorithmSimulator.h"


//static TTF_TextEngine* m_TextEngine{};

SpriteComponent::SpriteComponent(const std::string& file_path): m_ImagePath(file_path)
{
    m_Texture = AssetManager::GetInstance().GetImage(file_path);
    SDL_GetTextureSize(m_Texture, &m_SrcRect.w, &m_SrcRect.h);
    m_SpriteSize = Vec2f{m_SrcRect.w, m_SrcRect.h};
    m_DesRect = m_SrcRect;
    m_DesRect.w *= m_Scale.x;
    m_DesRect.h *= m_Scale.y;
    //std::cout<<"source rect: "<<m_SrcRect.w<<" "<<m_SrcRect.h<<std::endl;
}

CheckPointsComponent::CheckPointsComponent(const std::vector<Vec2f>& check_points, const SpriteComponent& sprite)
{
    m_CheckPoints = check_points;
    m_Sprite = sprite;
}

void SpriteComponent::SetScale(float scale_x, float scale_y)
{
    m_Scale = {scale_x, scale_y}; 
    m_DesRect.w = m_SpriteSize.x * m_Scale.x;
    m_DesRect.h = m_SpriteSize.y * m_Scale.y;
    m_SpriteSize.x *= m_Scale.x;
    m_SpriteSize.y *= m_Scale.y;
}

FirstAINeuralNetworkComponent& FirstAINeuralNetworkComponent::FeedForward() 
{
    //Random::Init();
    //m_ControlParameter = {1.0f, Random::Float()-0.5f};
    m_ControlParameter = {1.0f, Random::Float() - 0.5f};
    return *this;
}

TextComponent::TextComponent(const std::string& font_path, const std::string& text, int font_size): m_FontPath(font_path), m_TextString(text), m_FontSize(font_size)
{
    auto m_Font = AssetManager::GetInstance().GetFont(font_path, font_size);
    m_Text = TTF_CreateText(Simulator::GetTextEngine(), m_Font, m_TextString.c_str(), 0);
    TTF_SetTextColor(m_Text, m_Color.r, m_Color.g, m_Color.b, m_Color.a);
    TTF_SetTextFont(m_Text, m_Font);
}

TextComponent::~TextComponent()
{
    if (m_Text != nullptr)
    {
        TTF_DestroyText(m_Text);
    }
}

void TextComponent::SetFontSize(int font_size)
{
    m_FontSize = font_size;
    auto m_Font = AssetManager::GetInstance().GetFont(m_FontPath, font_size);
    TTF_SetTextFont(m_Text, m_Font);
}

void TextComponent::UpdateSize()
{
    TTF_GetTextSize(m_Text, &m_Width, &m_Height);
}

TextComponent& TextComponent::SetPosition(float x, float y)
{
    m_X = x;
    m_Y = y;
    return *this;
}

void TextComponent::SetColor(const Color &color)
{
    m_Color = color;
    TTF_SetTextColor(m_Text, m_Color.r, m_Color.g, m_Color.b, m_Color.a);
}

void TextComponent::HandleSpeedChange(float new_speed)
{
    std::string ui_message = "Speed: " + DecimalFormatTo<2>(new_speed);
    SetText(ui_message);
}

void TextComponent::HandlePopulationChange(int new_population)
{
    
    std::string ui_message = "Population: " + std::to_string(new_population) + "/" + std::to_string(GeneticAlgorithmSimulator::GetInstance().GetTotalCarsPopulation());
    SetText(ui_message);
}

void TextComponent::HandleRoundChange(int new_round)
{
    std::string ui_message = "Round: " + std::to_string(new_round);
    SetText(ui_message);
}

void TextComponent::SetText(const std::string& text, const Color& color)
{
    TTF_SetTextColor(m_Text, m_Color.r, m_Color.g, m_Color.b, m_Color.a);
    TTF_SetTextString(m_Text, text.c_str(), 0);
}

void Movement2DComponent::SetSpeed(float speed)
{
    if(m_Speed != speed)
    {
        m_OnSpeedChanged(speed);
        m_Speed = speed;
    }

}

void Movement2DComponent::Reset()
{
    m_MaxSpeed = 70.f;
	m_Acceleration = 0.f;
	m_Speed = 0.f;
	m_SlippingSpeed = m_MaxSpeed * 0.75f;
	m_IsShutOff = false;
	m_LastCheckPointPassed = 0;
}

void Movement2DComponent::HitCheckPoint(int checkPoint)
{
    //std::cout<<checkPoint<<std::endl;
    if((checkPoint - m_LastCheckPointPassed) == 1)
    {
        m_LastCheckPointPassed = checkPoint;
    }
    else if (checkPoint < m_LastCheckPointPassed)
    {
        m_IsShutOff = true;
    }
  
    
}

Layer::Layer(int input_size, int output_size) 
{
    int weight_size = input_size * output_size;
    m_Outputs = std::vector<float>(output_size, 0);

    m_Weights.resize(weight_size);

    for(int i = 0; i < weight_size; i++)
    {
        m_Weights[i] = 2.f * Random::Float() - 1.0f;
    }
}

void Layer::FeedForward(const std::vector<float>& inputs)
{
    int output_index = 0;
    int size = (int)inputs.size();
    for(auto& output : m_Outputs)
    {
        float sum = 0;
        int weight_index = 0;
        for(auto& input : inputs)
        {
            sum += input * m_Weights[output_index * size + weight_index];
            weight_index++;
        }
        output = std::tanh(sum);
        output_index++;
    }
}

AINeuralNetworkComponent::AINeuralNetworkComponent(const std::vector<int> &dimensions) 
{
    m_Dimensions = dimensions;
    m_Layers.reserve(m_Dimensions.size() - 1);
    for(int i = 0; i < m_Dimensions.size() - 1; i++)
    {
        //m_Layers[i].Layer(m_Dimensions[i], m_Dimensions[i + 1]);
        m_Layers.emplace_back(m_Dimensions[i], m_Dimensions[i + 1]);
    }
}

AINeuralNetworkComponent::AINeuralNetworkComponent(const std::vector<int> &dimensions, const std::vector<Layer> &layers)
{
    m_Dimensions = dimensions;
    m_Layers = layers;
}

std::vector<float> AINeuralNetworkComponent::FeedForward(const std::vector<float> &inputs)
{
    m_Inputs.clear();
    m_Inputs.reserve(inputs.size());
    m_Inputs.insert(m_Inputs.end(), inputs.begin(), inputs.end());
    std::vector<float> inputs_copy = inputs;
    for(auto& layer : m_Layers)
    {
        layer.FeedForward(inputs_copy);

        inputs_copy = layer.m_Outputs;
    }

    return std::move(inputs_copy);

}

std::vector<std::vector<float>> Evolution::Evolve(const std::vector<RankableChromosome>& chromosomes)
{
    //selection
    std::vector<RankableChromosome>sorted_chronomosomes(chromosomes.begin(), chromosomes.end());
    //sorted vector from highest to lowest
    std::sort(sorted_chronomosomes.begin(), sorted_chronomosomes.end(), std::greater<RankableChromosome>());
    // for(auto& chromosome : sorted_chronomosomes)
    // {
    //     std::cout<<chromosome.m_HighestCheckpoint<<std::endl;
    // }
    std::vector<std::vector<float>> sorted_chronomosomes_data;
    sorted_chronomosomes_data.reserve(sorted_chronomosomes.size());
    for(auto& chromosome : sorted_chronomosomes)
    {
        sorted_chronomosomes_data.push_back(chromosome.m_Chromosome);
    }

    std::vector keep_chromosomes(sorted_chronomosomes_data.begin(), sorted_chronomosomes_data.begin() + m_KeepCount);


    //cross over
    int reproduction_times = (m_PopulationCount - m_KeepCount) / m_KeepCount;

    std::vector offspring(keep_chromosomes.begin(), keep_chromosomes.end());

    for(int i = 0; i < reproduction_times; i++)
    {
        for(auto batch = 0; batch < m_KeepCount; batch += 2)
        {
            auto c1 = keep_chromosomes[batch];
            auto c2 = keep_chromosomes[batch + 1];
            int split_index = Random::UInt(0, c1.size() - 1);
            std::vector<float> new_child_1;
            std::vector<float> new_child_2;
            new_child_1.reserve(c1.size());
            new_child_1.insert(new_child_1.end(), c1.begin(), c1.begin() + split_index);
            new_child_1.insert(new_child_1.end(), c2.begin() + split_index, c2.end());

            new_child_2.reserve(c2.size());
            new_child_2.insert(new_child_2.end(), c2.begin(), c2.begin() + split_index);
            new_child_2.insert(new_child_2.end(), c1.begin() + split_index, c1.end());

            offspring.push_back(new_child_1); 
            offspring.push_back(new_child_2);

        }
       

    }
    
    //mutation
    for(auto i = m_KeepCount; i < offspring.size(); i++)
    {
        auto& child = offspring[i];
        for(auto& weight : child)
        {
            //20% mutation rate
            if(Random::UInt(0, 4) == 1)
            {
                weight = 2 * Random::Float() - 1;
            }
        }
    }

    return offspring;

}

void NeuronHUDDisplayComponent::FetchNeuronOutputsInEachLayer(const AINeuralNetworkComponent& ai_neural_network)
{
    m_NeuronValues.clear();

    int i = 0;
    for(auto& dim: ai_neural_network.m_Dimensions)
    {
        i += dim;
    }
    m_NeuronValues.reserve(i);

    m_NeuronValues.insert(m_NeuronValues.end(), ai_neural_network.m_Inputs.begin(), ai_neural_network.m_Inputs.end());

    for(auto& layer : ai_neural_network.m_Layers)
    {
        m_NeuronValues.insert(m_NeuronValues.end(), layer.m_Outputs.begin(), layer.m_Outputs.end());
    }

}

NeuronHUDDisplayComponent &NeuronHUDDisplayComponent::SetNeuronsPosition(const std::vector<Vec2f> &neurons_position)
{
    m_NeuronsPosition = neurons_position;
    return *this;
}


RankableChromosome AINeuralNetworkComponent::Serialize()
{
    std::vector<float> serialize_weights;
    int size = 0;
    for(auto& layer : m_Layers)
    {
        size += (int)layer.m_Weights.size();
    }
    serialize_weights.reserve(size);
    for(auto& layer : m_Layers)
    {
        serialize_weights.insert(serialize_weights.end(), layer.m_Weights.begin(), layer.m_Weights.end());
    }
    return {m_HighestCheckpoint, serialize_weights};
}

void AINeuralNetworkComponent::Deserialize(const std::vector<float>& chromosome)
{
    int offset = 0;
    int weight_size = 0;
    for(auto& layer : m_Layers)
    {
        weight_size = (int)layer.m_Weights.size();
        layer.m_Weights.clear();
        layer.m_Weights.insert(layer.m_Weights.end(), chromosome.begin() + offset, chromosome.begin() + offset + weight_size);
        offset += weight_size;
    }
}


RadarComponent::RadarComponent(float angle): m_Angle(angle)
{

}

Vec2f RadarComponent::CalculateEnd(const Vec2f& start, float angle, float distance)
{
    return start + Vec2f(std::cos(glm::radians(angle)), std::sin(glm::radians(angle))) * distance;
}

float RadarComponent::Probe(const SpriteComponent &track_sprite, const Transform2DComponent& transform)
{
    float probe_length = 0.f;
    m_Start = transform.m_Translation;
    m_End = m_Start;
    auto& simulator = GeneticAlgorithmSimulator::GetInstance();
    while(probe_length < m_MaxDistance && simulator.IsRoad(track_sprite, m_End))
    {
        probe_length += 2.f;
        m_End = CalculateEnd(m_Start, -(transform.m_Rotation + m_Angle), probe_length);
    }

    return probe_length;

}

RadarComponents::RadarComponents()
{
    m_RadarComponents.reserve(5);
  
    m_RadarComponents.emplace_back(-70.f);
    m_RadarComponents.emplace_back(-35.f);
    m_RadarComponents.emplace_back(0.f);
    m_RadarComponents.emplace_back(35.f);
    m_RadarComponents.emplace_back(70.f);
}

std::vector<float> RadarComponents::Probe(const SpriteComponent &track_sprite, const Transform2DComponent &transform)
{
    std::vector<float> probes;
    for(auto& radar : m_RadarComponents)
    {
        probes.push_back(radar.Probe(track_sprite, transform)/radar.m_MaxDistance);
    }
    return std::move(probes);
}
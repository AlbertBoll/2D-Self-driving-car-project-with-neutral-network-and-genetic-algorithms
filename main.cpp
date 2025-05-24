#include"GeneticAlgorithmSimulator.h"


int main(int, char**)
{
    
    WindowProperties window_properties;
    window_properties.m_Title = "Genetic Algorithm Simulator";
    window_properties.m_Width = 1280;
    window_properties.m_Height = 720;
    window_properties.flag = WindowFlags::NONE;
    auto& genetic_algorithm_simulator = GeneticAlgorithmSimulator::GetInstance().SetTotalSimulationRound(50);
    //ScopedPtr<Simulator> genetic_algorithm_simulator = CreateScopedPtr<GeneticAlgorithmSimulator>();
    genetic_algorithm_simulator.Initialize(window_properties);
    genetic_algorithm_simulator.Run();


}

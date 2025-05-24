#include"../include/Entity.h"
#include"../GeneticAlgorithmSimulator.h"


Entity::Entity(entt::entity handle, Scene* scene): m_EntityHandle(handle), m_Scene(scene)
{

}

Entity::~Entity()
{

}

void Entity::Update(float dt)
{
    if(HasAllComponents<Transform2DComponent, AINeuralNetworkComponent, SpriteComponent, Movement2DComponent, RadarComponents>())
    {   
        auto track = GeneticAlgorithmSimulator::GetInstance().GetTrack();
        auto& track_sprite = track.GetComponent<SpriteComponent>();
        auto& transform = GetComponent<Transform2DComponent>();
        auto& sprite = GetComponent<SpriteComponent>();
        auto& movement = GetComponent<Movement2DComponent>();
        auto measurements = GetComponent<RadarComponents>().Probe(track_sprite, transform);

        auto outputs = GetComponent<AINeuralNetworkComponent>().FeedForward(measurements);

        movement.m_Speed -= 0.5f; //friction
        float steer_impact{};
        if(!movement.m_IsShutOff)
        {
            float acceleration = outputs[0];
            float steer_position = outputs[1];
            float slipping_speed = movement.m_SlippingSpeed;

            if (acceleration > 0)
            {
                movement.SetSpeed(movement.GetSpeed() + 1.f);
            }
            
            if(movement.m_Speed > movement.m_MaxSpeed)
            {
                movement.SetSpeed(movement.m_MaxSpeed);
            }

            if(movement.m_Speed > slipping_speed)
            {
                steer_impact = -movement.m_Speed / movement.m_MaxSpeed + slipping_speed / movement.m_MaxSpeed + 1;
            }
            else
            {
                steer_impact = 1;
            }
            //movement.m_Speed = glm::clamp(movement.m_Speed, 0.0f, movement.m_MaxSpeed);
            transform.m_Rotation -= steer_position * steer_impact * movement.m_Speed * dt * 5;
        }
        else
        {
            //movement.m_Speed -= 0.05f * movement.m_Speed;
            movement.SetSpeed(movement.GetSpeed() - 0.05f * movement.GetSpeed());
        }

        if(movement.m_Speed < 1.f)
        {
            movement.m_IsShutOff = true;  
        }

        if(movement.m_Speed <= 0.f)
        {
            movement.SetSpeed(0.f);
        }


        //movement.m_Speed = glm::clamp(movement.m_Speed, 0.0f, movement.m_MaxSpeed);
        transform.m_Translation.x += movement.m_Speed * glm::cos(glm::radians(transform.m_Rotation)) * dt * 5;
        transform.m_Translation.y -= movement.m_Speed * glm::sin(glm::radians(transform.m_Rotation)) * dt * 5;
        sprite.m_Angle = -transform.m_Rotation;
        sprite.m_DesRect.x = transform.m_Translation.x - sprite.m_SpriteSize.x / 2;
        sprite.m_DesRect.y = transform.m_Translation.y - sprite.m_SpriteSize.y / 2;

    }

    
}

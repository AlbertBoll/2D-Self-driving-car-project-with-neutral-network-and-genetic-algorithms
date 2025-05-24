#pragma once

#include<entt/entt.hpp>
#include"UUID.h"
#include"Utility.h"
#include"Timestep.h"


class Entity;

class Scene
{
    friend class Entity;
public:
    Scene() = default;
    ~Scene() = default;
    void Update(Timestep ts);
    Entity CreateEntity(const std::string& name = std::string());
    Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
    void DestroyEntity(Entity entity);
    //void DestroyEntity(Entity entity, bool excludeChildren = false, bool first = true);
    //void DestroyEntity(UUID entityID, bool excludeChildren = false, bool first = true);
    entt::registry& Reg() { return m_Registry; }
    Entity DuplicateEntity(Entity entity);
    Entity FindEntityByName(std::string_view name);
    Entity GetEntityByUUID(UUID uuid);
    int GetEntityCount() { return m_EntityMap.size(); }
    std::unordered_map<UUID, entt::entity>& GetEntityMap();

    template<typename... Components>
    auto GetAllEntitiesWith()
    {
        return m_Registry.view<Components...>();
    }

    template<typename IncludeComponent, typename ... ExcludeComponents>
    auto GetAllEntitiesWithExclude()
    {
        return m_Registry.view<IncludeComponent>(entt::exclude<ExcludeComponents...>);
    }

    template<typename...Components>
    auto& View()
    {
        return m_Registry.view<Components...>();
    }

private:
    entt::registry m_Registry;
    std::unordered_map<UUID, entt::entity> m_EntityMap;
  
};



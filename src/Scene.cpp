#include "../include/Scene.h"
#include "../include/Entity.h"
#include "../include/Component.h"


template<typename... Components>
static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
{
    ([&]()
    {
        auto view = src.view<Components>();
        for (auto srcEntity : view)
        {
            auto dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);
            auto& srcComponent = src.get<Components>(srcEntity);
            dst.emplace_or_replace<Components>(dstEntity, srcComponent);
        }
    }(), ...);
}

template<typename... Components>
static void CopyComponent(ComponentsGroup<Components...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
{
	CopyComponent<Components...>(dst, src, enttMap);
}

template<typename... Components>
static void CopyComponentIfExists(Entity dst, Entity src)
{
	([&]()
		{
			if (src.HasAllComponents<Components>())
				dst.AddOrReplaceComponent<Components>(src.GetComponent<Components>());
		}(), ...);
}

template<typename... Components>
static void CopyComponentIfExists(ComponentsGroup<Components...>, Entity dst, Entity src)
{
	CopyComponentIfExists<Components...>(dst, src);
}

void Scene::Update(Timestep ts)
{
    
}

Entity Scene::CreateEntity(const std::string &tag)
{
    return CreateEntityWithUUID(UUID(), tag);
}

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string &name)
{
    Entity entity = { m_Registry.create(), this };
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<Transform2DComponent>();
    auto& tag = entity.AddComponent<TagComponent>();
    tag.Name = name.empty() ? "Entity" : name;
    m_EntityMap[uuid] = entity;
    return entity;
}

void Scene::DestroyEntity(Entity entity)
{
    m_EntityMap.erase(entity.GetUUID());
    m_Registry.destroy(entity);
}

Entity Scene::DuplicateEntity(Entity entity)
{
    // Copy name because we're going to modify component data structure
    std::string name = entity.GetName();
    Entity newEntity = CreateEntity(name);
    CopyComponentIfExists(AllComponents{}, newEntity, entity);
    // CopyComponentIfExists(AllComponents{}, newEntity, entity);

    // if (newEntity.HasAllComponents<RenderComponent>())
    // {
    //     auto shader_id = newEntity.GetComponent<RenderComponent>().Shader->GetHandle();
    //     m_GroupEntities[shader_id - 1].emplace_back(newEntity);
    //     //m_GroupEntities[shader_id - 1].emplace_back((entt::entity)newEntity, this);
    // }
    return newEntity;
}

Entity Scene::FindEntityByName(std::string_view name)
{
    auto view = m_Registry.view<TagComponent>();
    for (auto entity : view)
    {
        const TagComponent& tc = view.get<TagComponent>(entity);
        if (tc.Name == name) return { entity, this };
    }
    return {};
}

Entity Scene::GetEntityByUUID(UUID uuid)
{
    if (m_EntityMap.find(uuid) != m_EntityMap.end())
    {
        return { m_EntityMap[uuid], this };
    }

    return {};
}
std::unordered_map<UUID, entt::entity> &Scene::GetEntityMap()
{
    return m_EntityMap;
    // TODO: insert return statement here
}
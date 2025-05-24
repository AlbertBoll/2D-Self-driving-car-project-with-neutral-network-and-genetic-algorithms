#pragma once

#include<entt/entt.hpp>
#include "Scene.h"
#include "Component.h"

class Entity
{
public:
    Entity() = default;
    Entity(entt::entity handle, Scene* scene);
    ~Entity();
    Entity(const Entity& other) = default;

    template<typename... Args>
    bool HasAllComponents() const
    {
        return m_Scene->Reg().all_of<Args...>(m_EntityHandle);
    }

    template<typename T, typename ... Args>
    T& AddComponent(Args&&...args)
    {
        return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    T& GetComponent()
    {
        return m_Scene->m_Registry.get<T>(m_EntityHandle);
    }

    template<typename T>
    T GetComponent()const
    {
        return m_Scene->m_Registry.get<T>(m_EntityHandle);
    }

    template<typename ... Component>
    void RemoveComponents()
    {
        (m_Scene->m_Registry.remove<Component>(m_EntityHandle), ...);
    }

    template<typename Component>
    void RemoveComponent()
    {
        m_Scene->m_Registry.remove<Component>(m_EntityHandle);
    }


    template<typename T, typename ... Args>
    T& AddOrReplaceComponent(Args&& ... args)
    {
	    T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
	    //m_Scene->OnComponentAdded<T>(*this, component);
	    return component;
    }

    const std::string& GetName() { return GetComponent<TagComponent>().Name; }

    bool operator==(const Entity& other) const
    {
        return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
    }

    bool operator!=(const Entity& other) const
    {
        return !(*this == other);
    }

    operator uint32_t() const { return (uint32_t)m_EntityHandle; }   

    UUID GetUUID() 
    {
        return GetComponent<IDComponent>().ID; 
    }

    UUID GetParentUUID() const
    { 
        return GetComponent<RelationshipComponent>().ParentHandle; 
    }

    Entity GetParent() const
    {
        return m_Scene->GetEntityByUUID(GetParentUUID());
    }

    void SetParentUUID(UUID parent) { GetComponent<RelationshipComponent>().ParentHandle = parent; }

    void SetParent(Entity parent)
    {
        Entity currentParent = GetParent();
        if (currentParent == parent)
            return;

        // If changing parent, remove child from existing parent
        if (currentParent)
            currentParent.RemoveChild(*this);

        // Setting to null is okay
        SetParentUUID(parent.GetUUID());

        if (parent)
        {
            auto& parentChildren = parent.Children();
            UUID uuid = GetUUID();
            if (std::find(parentChildren.begin(), parentChildren.end(), uuid) == parentChildren.end())
                parentChildren.emplace_back(GetUUID());
        }
    }

    std::vector<UUID>& Children() { return GetComponent<RelationshipComponent>().Children; }
    const std::vector<UUID>& Children() const { return GetComponent<RelationshipComponent>().Children; }

    bool RemoveChild(Entity child)
    {
        UUID childId = child.GetUUID();
        std::vector<UUID>& children = Children();
        auto it = std::find(children.begin(), children.end(), childId);
        if (it != children.end())
        {
            children.erase(it);
            return true;
        }
    
        return false;
    }

    bool IsAncesterOf(Entity entity) const
    {
        const auto& children = Children();

        if (children.empty())
            return false;

        for (UUID child : children)
        {
            if (child == entity.GetUUID())
                return true;
        }

        for (UUID child : children)
        {
            if (m_Scene->GetEntityByUUID(child).IsAncesterOf(entity))
                return true;
        }

        return false;
    }

    void Update(float dt);
    
    Scene* GetSceneContext()const { return m_Scene; }

    bool IsDescendentOf(Entity entity) const
    {
        return entity.IsAncesterOf(*this);
    }

    operator entt::entity() { return m_EntityHandle; }

    std::string& Name() { return HasAllComponents<TagComponent>() ? GetComponent<TagComponent>().Name : NoName; }
    const std::string& Name() const { return HasAllComponents<TagComponent>() ? GetComponent<TagComponent>().Name : NoName; }

    Transform2DComponent& Transform() { return m_Scene->m_Registry.get<Transform2DComponent>(m_EntityHandle); }
   

private:
    entt::entity m_EntityHandle{entt::null};
    Scene* m_Scene{};
    inline static std::string NoName = "Unnamed";
};
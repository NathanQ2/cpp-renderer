#include "ptpch.h"
#include "EntityComponentSystem.h"

namespace PalmTree {
    EntityComponentSystem::EntityComponentSystem() {
        m_ComponentManager.RegisterComponent<TransformComponent>();
        m_ComponentManager.RegisterComponent<PointLightComponent>();
        m_ComponentManager.RegisterComponent<ModelComponent>();

        m_GameObjects.reserve(MAX_GAME_OBJECTS);
    }

    GameObject& EntityComponentSystem::CreateGameObject(TransformComponent transform) {
        Id id = m_EntityManager.CreateGameObject();

        AddComponent<TransformComponent>(id, transform);

        m_GameObjects.emplace_back(id, m_EntityManager.GetSignature(id), this);

        return m_GameObjects[id];
    }

    GameObject& EntityComponentSystem::GetObject(Id id) {
        return m_GameObjects[id];
    }
}

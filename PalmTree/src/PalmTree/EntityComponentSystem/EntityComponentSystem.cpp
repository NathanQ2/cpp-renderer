#include "ptpch.h"
#include "EntityComponentSystem.h"

namespace PalmTree {
    EntityComponentSystem::EntityComponentSystem() {
        m_ComponentManager.RegisterComponent<TransformComponent>();
        m_ComponentManager.RegisterComponent<PointLightComponent>();
        m_ComponentManager.RegisterComponent<ModelComponent>();
        m_ComponentManager.RegisterComponent<ColliderComponent>();
        m_ComponentManager.RegisterComponent<RigidBodyComponent>();
    }

    GameObject& EntityComponentSystem::CreateGameObject(TransformComponent transform) {
        Id id = m_EntityManager.CreateGameObject();

        AddComponent<TransformComponent>(id, transform);

        m_GameObjects[id] = GameObject(id, this);

        return m_GameObjects[id];
    }

    GameObject& EntityComponentSystem::GetObject(Id id) {
        return m_GameObjects[id];
    }

    std::vector<GameObject> EntityComponentSystem::GetGameObjects() {
        std::vector<GameObject> objs;

        // TDOO: Consider reserving only valid game objects
        objs.reserve(m_GameObjects.size());

        for (GameObject& obj : m_GameObjects) {
            if (obj.IsValid())
                objs.emplace_back(obj);
        }

        return objs;
    }
}

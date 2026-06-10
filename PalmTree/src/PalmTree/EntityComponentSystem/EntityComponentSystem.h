#pragma once

#include "Components.h"
#include "GameObject.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"

#include <vector>

namespace PalmTree {
    class EntityComponentSystem {
    public:
        EntityComponentSystem();

        GameObject& CreateGameObject(TransformComponent transform = TransformComponent{});
        GameObject& GetObject(Id id);

        template <typename T>
        void RegisterComponent() {
            m_ComponentManager.RegisterComponent<T>();
        }

        template <typename T>
        void AddComponent(Id id, T component) {
            m_ComponentManager.AddComponent<T>(id, component);
            m_EntityManager.SetSignatureBit(id, m_ComponentManager.GetComponentType<T>(), true);
            m_SystemManager.GameObjectSignatureChanged(id, m_EntityManager.GetSignature(id));
        }

        template <typename T>
        void RemoveComponent(Id id) {
            m_ComponentManager.RemoveComponent<T>(id);
            m_EntityManager.SetSignatureBit(id, m_ComponentManager.GetComponentType<T>(), false);
            m_SystemManager.GameObjectSignatureChanged(id, m_EntityManager.GetSignature(id));
        }

        template <typename T>
        T& GetComponent(Id id) {
            return m_ComponentManager.GetComponent<T>(id);
        }

        template <typename T>
        ComponentType GetComponentType() {
            return m_ComponentManager.GetComponentType<T>();
        }

        template <typename T>
        void RegisterSystem(std::shared_ptr<T> system, Signature signature) {
            m_SystemManager.RegisterSystem<T>(system, signature, this);
            size_t numObjects = m_EntityManager.GetGameObjectsCount();

            for (size_t i = 0; i < numObjects; i++) {
                m_SystemManager.InitSystemWithObject<T>(i, m_EntityManager.GetSignature(i));
            }
        }

        ComponentManager& GetComponentManager() { return m_ComponentManager; }
    private:
        EntityManager m_EntityManager{};
        ComponentManager m_ComponentManager{};
        SystemManager m_SystemManager{};

        std::vector<GameObject> m_GameObjects{};
    };

    template <typename T>
    T& GameObject::GetComponent() { return m_Ecs->GetComponent<T>(m_Id); }

    template <typename T>
    void GameObject::AddComponent(T component) { m_Ecs->AddComponent<T>(m_Id, component); }

    inline TransformComponent& GameObject::GetTransform() { return GetComponent<TransformComponent>(); }
}

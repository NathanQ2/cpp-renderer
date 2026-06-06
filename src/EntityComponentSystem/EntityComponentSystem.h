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

        GameObject& createGameObject(TransformComponent transform = TransformComponent{});
        GameObject& getObject(Id id);

        template <typename T>
        void registerComponent() {
            m_componentManager.registerComponent<T>();
        }

        template <typename T>
        void addComponent(Id id, T component) {
            m_componentManager.addComponent<T>(id, component);
            m_entityManager.setSignatureBit(id, m_componentManager.getComponentType<T>(), true);
            m_systemManager.gameObjectSignatureChanged(id, m_entityManager.getSignature(id));
        }

        template <typename T>
        void removeComponent(Id id) {
            m_componentManager.removeComponent<T>(id);
            m_entityManager.setSignatureBit(id, m_componentManager.getComponentType<T>(), false);
            m_systemManager.gameObjectSignatureChanged(id, m_entityManager.getSignature(id));
        }

        template <typename T>
        T& getComponent(Id id) {
            return m_componentManager.getComponent<T>(id);
        }

        template <typename T>
        ComponentType getComponentType() {
            return m_componentManager.getComponentType<T>();
        }

        template <typename T>
        void registerSystem(std::shared_ptr<T> system, Signature signature) {
            m_systemManager.registerSystem<T>(system, signature, this);
            size_t numObjects = m_entityManager.getGameObjectsCount();

            for (size_t i = 0; i < numObjects; i++) {
                m_systemManager.initSystemWithObject<T>(i, m_entityManager.getSignature(i));
            }
        }

        ComponentManager& getComponentManager() { return m_componentManager; }
    private:
        EntityManager m_entityManager{};
        ComponentManager m_componentManager{};
        SystemManager m_systemManager{};

        std::vector<GameObject> m_gameObjects{};
    };

    template <typename T>
    T& GameObject::getComponent() { return m_ecs->getComponent<T>(m_id); }

    template <typename T>
    void GameObject::addComponent(T component) { m_ecs->addComponent<T>(m_id, component); }

    inline TransformComponent& GameObject::getTransform() { return getComponent<TransformComponent>(); }
}

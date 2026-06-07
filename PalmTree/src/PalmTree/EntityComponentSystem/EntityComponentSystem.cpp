#include "EntityComponentSystem.h"

namespace PalmTree {
    EntityComponentSystem::EntityComponentSystem() {
        m_componentManager.registerComponent<TransformComponent>();
        m_componentManager.registerComponent<PointLightComponent>();
        m_componentManager.registerComponent<ModelComponent>();

        m_gameObjects.reserve(MAX_GAME_OBJECTS);
    }

    GameObject& EntityComponentSystem::createGameObject(TransformComponent transform) {
        Id id = m_entityManager.createGameObject();

        addComponent<TransformComponent>(id, transform);

        m_gameObjects.emplace_back(id, m_entityManager.getSignature(id), this);

        return m_gameObjects[id];
    }

    GameObject& EntityComponentSystem::getObject(Id id) {
        return m_gameObjects[id];
    }
}

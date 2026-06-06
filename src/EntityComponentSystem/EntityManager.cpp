#include "EntityManager.h"

#include <cassert>

namespace PalmTree {
    EntityManager::EntityManager() {
        for (Id i = 0; i < MAX_GAME_OBJECTS; i++) {
            m_availableGameObjects.push(i);
        }
    }

    Id EntityManager::createGameObject() {
        assert(m_gameObjectsCount < MAX_GAME_OBJECTS && "Too many game objects!");

        const Id id = m_availableGameObjects.front();
        m_availableGameObjects.pop();
        m_gameObjectsCount++;

        return id;
    }

    void EntityManager::destroyGameObject(Id id) {
        m_availableGameObjects.push(id);
        m_gameObjectsCount--;
    }
}

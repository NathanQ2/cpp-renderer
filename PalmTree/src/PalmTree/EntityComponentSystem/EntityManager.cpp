#include "ptpch.h"
#include "EntityManager.h"


namespace PalmTree {
    EntityManager::EntityManager() {
        for (Id i = 0; i < MAX_GAME_OBJECTS; i++) {
            m_AvailableGameObjects.push(i);
        }
    }

    Id EntityManager::CreateGameObject() {
        assert(m_GameObjectsCount < MAX_GAME_OBJECTS && "Too many game objects!");

        const Id id = m_AvailableGameObjects.front();
        m_AvailableGameObjects.pop();
        m_GameObjectsCount++;

        return id;
    }

    void EntityManager::DestroyGameObject(Id id) {
        m_AvailableGameObjects.push(id);
        m_GameObjectsCount--;
    }
}

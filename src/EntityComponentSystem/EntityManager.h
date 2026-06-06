#pragma once

#include "Types.h"

#include <array>
#include <queue>

namespace PalmTree {
    class EntityManager {
    public:
        EntityManager();

        Id createGameObject();
        void destroyGameObject(Id id);

        void setSignatureBit(Id id, ComponentType bit, bool value) {
            m_signatures[id].set(bit, value);
        }


        [[nodiscard]] const Signature& getSignature(Id id) const { return m_signatures[id]; }
        [[nodiscard]] size_t getGameObjectsCount() const { return m_gameObjectsCount; }

    private:
        std::queue<Id> m_availableGameObjects;

        size_t m_gameObjectsCount = 0;

        std::array<Signature, MAX_GAME_OBJECTS> m_signatures{};
    };
}

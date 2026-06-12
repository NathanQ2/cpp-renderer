#pragma once

#include "Types.h"

#include <queue>

namespace PalmTree {
    class EntityManager {
    public:
        EntityManager();

        Id CreateGameObject();
        void DestroyGameObject(Id id);

        void SetSignatureBit(Id id, ComponentType bit, bool value) {
            m_Signatures[id].set(bit, value);
        }


        [[nodiscard]] const Signature& GetSignature(Id id) const { return m_Signatures[id]; }
        [[nodiscard]] size_t GetGameObjectsCount() const { return m_GameObjectsCount; }

    private:
        std::queue<Id> m_AvailableGameObjects;

        size_t m_GameObjectsCount = 0;

        std::array<Signature, MAX_GAME_OBJECTS> m_Signatures{};
    };
}

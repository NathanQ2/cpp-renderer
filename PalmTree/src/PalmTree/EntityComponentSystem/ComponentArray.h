#pragma once

#include "Types.h"

namespace PalmTree {
    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
    };

    template <typename T>
    class ComponentArray : public IComponentArray {
    public:
        void AddComponent(Id id, const T& component) { m_Components[id] = component; }
        void RemoveComponent(Id id) { m_Components[id] = T{}; }

        T& GetComponent(Id id) { return m_Components[id]; }

    private:
        std::array<T, MAX_GAME_OBJECTS> m_Components{};
    };
}

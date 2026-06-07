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
        void addComponent(Id id, const T& component) { m_components[id] = component; }
        void removeComponent(Id id) { m_components[id] = T{}; }

        T& getComponent(Id id) { return m_components[id]; }

    private:
        std::array<T, MAX_GAME_OBJECTS> m_components{};
    };
}

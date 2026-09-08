#pragma once

#include "Types.h"

#include <set>

namespace PalmTree {
    class EntityComponentSystem;

    class System {
        friend class SystemManager;
    public:
        virtual ~System() = default;

        virtual void OnRegistered() {}
    protected:
        std::set<Id> m_Ids;

        EntityComponentSystem* m_Ecs = nullptr;
    };
}

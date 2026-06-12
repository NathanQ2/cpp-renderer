#pragma once

#include "Types.h"

#include <set>

namespace PalmTree {
    class EntityComponentSystem;

    class System {
    public:
        friend class SystemManager;

    protected:
        std::set<Id> m_Ids;

        EntityComponentSystem* m_Ecs = nullptr;
    };
}

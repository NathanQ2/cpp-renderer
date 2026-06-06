#pragma once

#include "Types.h"

#include <set>

namespace PalmTree {
    class EntityComponentSystem;
   
    class System {
    public:
        friend class SystemManager;

    protected:
        std::set<Id> m_ids;

        EntityComponentSystem* m_ecs = nullptr;
    };
}

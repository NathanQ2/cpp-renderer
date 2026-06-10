#pragma once

#include "Components.h"
#include "Types.h"

namespace PalmTree {
    class EntityComponentSystem;

    class GameObject {
    public:
        friend class EntityManager;
        friend class EntityComponentSystem;

        explicit GameObject(
            Id id,
            const Signature& signature,
            EntityComponentSystem* ecs
        ) : m_Id(id), m_Signature(signature), m_Ecs(ecs) {}

        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = default;
        GameObject& operator=(GameObject&& other) = delete;

        [[nodiscard]] Id GetId() const { return m_Id; }


        // Implemented in EntityComponentSystem.h
        template <typename T>
        T& GetComponent();

        // Implemented in EntityComponentSystem.h
        template <typename T>
        void AddComponent(T component);

        // Implemented in EntityComponentSystem.h
        TransformComponent& GetTransform();

    private:
        Id m_Id;
        const Signature& m_Signature;
        EntityComponentSystem* m_Ecs;
    };
}

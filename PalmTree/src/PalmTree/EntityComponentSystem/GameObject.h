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
        ) : m_id(id), m_signature(signature), m_ecs(ecs) {}

        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = default;
        GameObject& operator=(GameObject&& other) = delete;

        [[nodiscard]] Id getId() const { return m_id; }


        // Implemented in EntityComponentSystem.h
        template <typename T>
        T& getComponent();

        // Implemented in EntityComponentSystem.h
        template <typename T>
        void addComponent(T component);

        // Implemented in EntityComponentSystem.h
        TransformComponent& getTransform();

    private:
        Id m_id;
        const Signature& m_signature;
        EntityComponentSystem* m_ecs;
    };
}

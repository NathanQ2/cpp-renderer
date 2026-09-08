#pragma once

#include "Components.h"
#include "Types.h"
#include "../Logging/Log.h"

namespace PalmTree {
    class EntityComponentSystem;

    class GameObject {
    public:
        friend class EntityManager;
        friend class EntityComponentSystem;

        GameObject() : m_Id(0), m_Ecs(nullptr), m_IsValid(false) {}
        GameObject(Id id, EntityComponentSystem* ecs) : m_Id(id), m_Ecs(ecs), m_IsValid(true) {}

        GameObject(const GameObject& other) {
            m_Id = other.m_Id;
            m_Ecs = other.m_Ecs;
            m_IsValid = other.m_IsValid;
        }

        GameObject& operator=(const GameObject& other) {
            m_Id = other.m_Id;
            m_Ecs = other.m_Ecs;
            m_IsValid = other.m_IsValid;

            return *this;
        }

        GameObject(GameObject&& other) noexcept {
            m_Id = other.m_Id;
            m_Ecs = other.m_Ecs;
            m_IsValid = other.m_IsValid;

            other.m_IsValid = false;
        }

        GameObject& operator=(GameObject&& other) {
            m_Id = other.m_Id;
            m_Ecs = other.m_Ecs;
            m_IsValid = other.m_IsValid;

            other.m_IsValid = false;

            return *this;
        }

        [[nodiscard]] Id GetId() const {
            PT_CORE_ASSERT(m_IsValid, "GameObject is not valid!");
            return m_Id;
        }


        // Implemented in EntityComponentSystem.h
        template<typename T>
        T* GetComponent();

        // Implemented in EntityComponentSystem.h
        template<typename T>
        void AddComponent(T component);

        template<typename T>
        bool HasComponent();

        // Implemented in EntityComponentSystem.h
        TransformComponent* GetTransform();

        bool IsValid() const { return m_IsValid; }
    private:
        Id m_Id;
        EntityComponentSystem* m_Ecs;

        bool m_IsValid;
    };
}

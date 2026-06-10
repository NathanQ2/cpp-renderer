#pragma once

#include <unordered_map>
#include <string_view>
#include <cassert>
#include <print>
#include <typeinfo>

#include "ComponentArray.h"
#include "Components.h"

namespace PalmTree {
    class ComponentManager {
    public:
        template <typename T>
        void RegisterComponent() {
            const char* name = typeid(T).name();

            assert(!m_ComponentTypes.contains(name) && "Component has already been registered!");
            assert(m_ComponentTypes.size() < MAX_COMPONENTS && "Too many components registered!");

            ComponentType type = m_NextComponentType;
            m_ComponentTypes[name] = type;
            m_NextComponentType++;

            m_ComponentArrays[type] = std::make_shared<ComponentArray<T>>();
        }

        template <typename T>
        void AddComponent(Id id, const T& component) {
            GetComponentArray<T>()->AddComponent(id, component);
        }

        template <typename T>
        void RemoveComponent(Id id) {
            GetComponentArray<T>()->RemoveComponent(id);
        }

        template <typename T>
        T& GetComponent(Id id) {
            return GetComponentArray<T>()->GetComponent(id);
        }

        template <typename T>
        ComponentType GetComponentType() {
            const char* name = typeid(T).name();

            assert(m_ComponentTypes.contains(name) && "Component has not been registered!");

            return m_ComponentTypes[name];
        }

    private:
        std::unordered_map<std::string_view, ComponentType> m_ComponentTypes{};
        ComponentType m_NextComponentType = 0;

        std::unordered_map<ComponentType, std::shared_ptr<IComponentArray>> m_ComponentArrays{};

        template <typename T>
        std::shared_ptr<ComponentArray<T>> GetComponentArray() {
            ComponentType type = GetComponentType<T>();

            assert(m_ComponentArrays.contains(type) && "Component has not been registered!");

            return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[type]);
        }
    };

    template <typename... Ts>
    class SignatureBuilder {
    public:
        SignatureBuilder(ComponentManager& cm) : m_Cm(cm) {
            (Add<Ts>(), ...);
        }

        template <typename U>
        SignatureBuilder& Add() {
            m_Signature.set(m_Cm.GetComponentType<U>(), true);

            return *this;
        }

        template <typename U>
        SignatureBuilder& Aemove() {
            m_Signature.set(m_Cm.GetComponentType<U>(), false);

            return *this;
        }

        [[nodiscard]] Signature Build() const {
            return m_Signature;
        }

    private:
        Signature m_Signature{};

        ComponentManager& m_Cm;
    };
}

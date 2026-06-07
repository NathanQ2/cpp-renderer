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
        void registerComponent() {
            const char* name = typeid(T).name();

            assert(!m_componentTypes.contains(name) && "Component has already been registered!");
            assert(m_componentTypes.size() < MAX_COMPONENTS && "Too many components registered!");

            ComponentType type = m_nextComponentType;
            m_componentTypes[name] = type;
            m_nextComponentType++;

            m_componentArrays[type] = std::make_shared<ComponentArray<T>>();
        }

        template <typename T>
        void addComponent(Id id, const T& component) {
            getComponentArray<T>()->addComponent(id, component);
        }

        template <typename T>
        void removeComponent(Id id) {
            getComponentArray<T>()->removeComponent(id);
        }

        template <typename T>
        T& getComponent(Id id) {
            return getComponentArray<T>()->getComponent(id);
        }

        template <typename T>
        ComponentType getComponentType() {
            const char* name = typeid(T).name();

            assert(m_componentTypes.contains(name) && "Component has not been registered!");

            return m_componentTypes[name];
        }

    private:
        std::unordered_map<std::string_view, ComponentType> m_componentTypes{};
        ComponentType m_nextComponentType = 0;

        std::unordered_map<ComponentType, std::shared_ptr<IComponentArray>> m_componentArrays{};

        template <typename T>
        std::shared_ptr<ComponentArray<T>> getComponentArray() {
            ComponentType type = getComponentType<T>();

            assert(m_componentArrays.contains(type) && "Component has not been registered!");

            return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[type]);
        }
    };

    template <typename... Ts>
    class SignatureBuilder {
    public:
        SignatureBuilder(ComponentManager& cm) : m_cm(cm) {
            (add<Ts>(), ...);
        }

        template <typename U>
        SignatureBuilder& add() {
            m_signature.set(m_cm.getComponentType<U>(), true);

            return *this;
        }

        template <typename U>
        SignatureBuilder& remove() {
            m_signature.set(m_cm.getComponentType<U>(), false);

            return *this;
        }

        [[nodiscard]] Signature build() const {
            return m_signature;
        }

    private:
        Signature m_signature{};

        ComponentManager& m_cm;
    };
}

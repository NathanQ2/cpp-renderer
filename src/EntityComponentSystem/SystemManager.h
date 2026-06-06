#pragma once

#include "System.h"

#include <unordered_map>
#include <typeinfo>
#include <cassert>
#include <memory>

namespace PalmTree {
    class SystemManager {
    public:
        template <typename T>
        void registerSystem(std::shared_ptr<T> system, Signature signature, EntityComponentSystem* ecs) {
            const char* name = typeid(T).name();

            assert(!m_systems.contains(name) && "The system has already been registered!");

            system->m_ecs = ecs;

            m_systems[name] = system;
            m_signatures[name] = signature;
        }

        /**
         * Adds the object's id to the system if their signatures match. 
         * Used after a call to registerSystem() to fill initialize they system with objects.
         * @tparam T The Type of the system
         * @param id The id of the object to add (if their signatures match)
         * @param objSignature The signature of the object
         */
        template <typename T>
        void initSystemWithObject(Id id, Signature objSignature) {
            const char* name = typeid(T).name();

            std::shared_ptr<System> system = m_systems[name];
            Signature systemSignature = m_signatures[name];

            if ((objSignature & systemSignature) == systemSignature) {
                system->m_ids.insert(id);
            }
        }

        void gameObjectSignatureChanged(Id id, Signature objSignature) {
            for (const auto& kv : m_systems) {
                std::shared_ptr<System> system = kv.second;
                Signature signature = m_signatures[kv.first];

                if ((objSignature & signature) == signature) {
                    system->m_ids.insert(id);
                }
                else {
                    system->m_ids.erase(id);
                }
            }
        }

    private:
        std::unordered_map<const char*, std::shared_ptr<System>> m_systems{};
        std::unordered_map<const char*, Signature> m_signatures{};
    };
}

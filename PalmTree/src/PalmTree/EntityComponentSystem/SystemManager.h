#pragma once

#include "System.h"

#include <typeinfo>

namespace PalmTree {
    class SystemManager {
    public:
        template <typename T>
        void RegisterSystem(std::shared_ptr<T> system, Signature signature, EntityComponentSystem* ecs) {
            const char* name = typeid(T).name();

            assert(!m_Systems.contains(name) && "The system has already been registered!");

            system->m_Ecs = ecs;

            m_Systems[name] = system;
            m_Signatures[name] = signature;
        }

        /**
         * Adds the object's id to the system if their signatures match. 
         * Used after a call to registerSystem() to fill initialize they system with objects.
         * @tparam T The Type of the system
         * @param id The id of the object to add (if their signatures match)
         * @param objSignature The signature of the object
         */
        template <typename T>
        void InitSystemWithObject(Id id, Signature objSignature) {
            const char* name = typeid(T).name();

            std::shared_ptr<System> system = m_Systems[name];
            Signature systemSignature = m_Signatures[name];

            if ((objSignature & systemSignature) == systemSignature) {
                system->m_Ids.insert(id);
            }
        }

        void GameObjectSignatureChanged(Id id, Signature objSignature) {
            for (const auto& kv : m_Systems) {
                std::shared_ptr<System> system = kv.second;
                Signature signature = m_Signatures[kv.first];

                if ((objSignature & signature) == signature) {
                    system->m_Ids.insert(id);
                }
                else {
                    system->m_Ids.erase(id);
                }
            }
        }

    private:
        std::unordered_map<const char*, std::shared_ptr<System>> m_Systems{};
        std::unordered_map<const char*, Signature> m_Signatures{};
    };
}

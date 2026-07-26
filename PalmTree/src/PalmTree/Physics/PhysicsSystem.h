#pragma once

#include <glm/glm.hpp>

#include "CollisionSystem.h"
#include "PalmTree/EntityComponentSystem/System.h"
#include "PalmTree/EntityComponentSystem/GameObject.h"

namespace PalmTree {
    class PhysicsSystem : public System {
    public:
        void OnRegistered() override;
        
        void Update(float dt);

        void AddForce(Id id, glm::vec3 force, glm::vec3 offset = glm::vec3{0.0f});
        void AddForce(GameObject& obj, glm::vec3 force, glm::vec3 offset = glm::vec3{0.0f});
        
        void AddImpulse(Id, glm::vec3 impulse);
        
        void OnImGuiRender();
    private:
        #ifdef PT_DEBUG
        struct ObjectDebugInfo {
            Id Id;
            uint64_t Step;
            double Time;
            
            std::shared_ptr<TransformComponent> Transform = nullptr;
            std::shared_ptr<RigidBodyComponent> RigidBody = nullptr;
            std::shared_ptr<ColliderComponent> Collider = nullptr;
            
            std::vector<std::pair<glm::vec3, glm::vec3>> Forces;
            std::vector<glm::vec3> Impulses;
        };
        
        std::unordered_map<Id, ObjectDebugInfo> m_DebugInfo;
        #endif
        
        const float SIMULATION_FREQUENCY = 1000.0f;
        const float STEP_SIZE = 1.0f / SIMULATION_FREQUENCY;
        float m_TimeAccum = 0.0f;
        float m_TimeScale = 1.0f;
        
        std::shared_ptr<CollisionSystem> m_CollisionSystem;
        
        uint64_t m_StepCount = 0;
        
        bool m_Paused = true;

        void Step(float dt);
    };
}

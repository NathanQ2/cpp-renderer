#pragma once

#include <glm/glm.hpp>

#include "CollisionSystem.h"
#include "PalmTree/EntityComponentSystem/System.h"
#include "PalmTree/EntityComponentSystem/GameObject.h"
#include "PalmTree/Logging/DataLogger.h"

namespace PalmTree {
    class PhysicsSystem : public System {
    public:
        const float GRAVITATIONAL_ACCELERATION = 9.80665f;

        void OnRegistered() override;

        void Update(float dt);

        void AddForce(Id id, glm::vec3 force, glm::vec3 offset = glm::vec3{0.0f});
        void AddForce(GameObject& obj, glm::vec3 force, glm::vec3 offset = glm::vec3{0.0f});

        void AddImpulse(Id, glm::vec3 impulse);

        void OnImGuiRender();
    private:
        DataLogger m_Logger{"/PhysicsSystem"};

        const float SIMULATION_FREQUENCY = 1000.0f;
        const float STEP_SIZE = 1.0f / SIMULATION_FREQUENCY;
        float m_TimeAccum = 0.0f;
        float m_TimeScale = 1.0f;

        std::shared_ptr<CollisionSystem> m_CollisionSystem;

        uint64_t m_StepCount = 0;

        bool m_Paused = true;

        static LogPath ObjectLogPath(Id id) {
            return LogPath("RigidBodies") / LogPath(fmt::format("{}", id));
        }

        void Step(float dt);

        void LogTransform(Id id, const TransformComponent* transform);
        void LogRigidbody(Id id, const RigidBodyComponent* rb);
        void LogCollider(Id id, const ColliderComponent* col);
    };
}

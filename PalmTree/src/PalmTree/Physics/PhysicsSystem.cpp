// Implementation based off derivation from blackedout01 on YouTube https://youtu.be/4r_EvmPKOvY

#include "PhysicsSystem.h"

#include <glm/gtx/string_cast.hpp>

#include "implot.h"
#include "PalmTree/EntityComponentSystem/EntityComponentSystem.h"

namespace PalmTree {
    void PhysicsSystem::OnRegistered() {
        m_CollisionSystem = m_Ecs->GetSystem<CollisionSystem>();
        PT_CORE_ASSERT(m_CollisionSystem, "CollisionSystem must be registered before PhysicsSystem");
    }

    void PhysicsSystem::Update(float dt) {
        if (m_Paused) return;

        m_TimeAccum += dt;
        while (m_TimeAccum > STEP_SIZE) {
            Step(STEP_SIZE * m_TimeScale);
            m_TimeAccum -= STEP_SIZE;
        }
    }

    void PhysicsSystem::AddForce(Id id, glm::vec3 force, glm::vec3 offset) {
        RigidBodyComponent* rb = m_Ecs->GetComponent<RigidBodyComponent>(id);
        PT_CORE_ASSERT(rb, "Object must have a rigidbody component to apply force!");

        rb->Acceleration += force / rb->Mass;

        glm::vec3 torque = glm::cross(offset, force);
        rb->AngularMomentum += torque * STEP_SIZE;
    }

    void PhysicsSystem::AddForce(GameObject& obj, glm::vec3 force, glm::vec3 offset) {
        AddForce(obj.GetId(), force, offset);
    }

    void PhysicsSystem::AddImpulse(Id id, glm::vec3 impulse) {
        RigidBodyComponent* rb = m_Ecs->GetComponent<RigidBodyComponent>(id);
        PT_CORE_ASSERT(rb, "Object must have a rigidbody component to apply impulse!");
        rb->Velocity += impulse / rb->Mass;
    }

    void PhysicsSystem::OnImGuiRender() {
        ImPlot::ShowDemoWindow();

        ImGui::Begin("PhysicsDebug");
        if (ImGui::Button(m_Paused ? "Resume" : "Pause")) {
            m_Paused = !m_Paused;
        }

        if (ImGui::Button("Step")) {
            Step(STEP_SIZE);
        }

        ImGui::DragFloat("Timescale", &m_TimeScale, 0.05, 0, 2);

        ImGui::Text("Step: %llu", m_StepCount);
        ImGui::Text("Time: %lfs", m_StepCount * (double)STEP_SIZE);

        ImGui::Separator();

        float totalKineticEnergy = 0.0f;
        float totalPotentialEnergy = 0.0f;
        for (Id id : m_Ids) {
            TransformComponent* t = m_Ecs->GetComponent<TransformComponent>(id);
            RigidBodyComponent* rb = m_Ecs->GetComponent<RigidBodyComponent>(id);
            PT_CORE_ASSERT(rb, "Object must have a rigidbody");
            totalKineticEnergy += 0.5f * rb->Mass * glm::length(rb->Velocity);

            if (rb->EnableGravity) totalPotentialEnergy += rb->Mass * t->Translation.y * -1;
        }
        float totalEnergy = totalKineticEnergy + totalPotentialEnergy;
        ImGui::Text("Total Kinetic Energy: %f", totalKineticEnergy);
        ImGui::Text("Total Potential Energy: %f", totalPotentialEnergy);
        ImGui::Text("Total Energy: %f", totalEnergy);

        ImGui::End();
    }

    void PhysicsSystem::Step(float dt) {
        if (dt == 0.0f) return;

        m_CollisionSystem->Update();

        for (Id id : m_Ids) {
            TransformComponent* transform = m_Ecs->GetComponent<TransformComponent>(id);
            RigidBodyComponent* rb = m_Ecs->GetComponent<RigidBodyComponent>(id);

            if (rb->EnableGravity) {
                AddForce(id, glm::vec3(0.0f, GRAVITATIONAL_ACCELERATION * rb->Mass, 0.0f));
            }

            std::vector<CollisionSystem::CollisionInfo>& infos = m_CollisionSystem->GetCollisionInfos(id);
            for (auto& info : infos) {
                TransformComponent* t2 = m_Ecs->GetComponent<TransformComponent>(info.OtherId);
                RigidBodyComponent* rb2 = m_Ecs->GetComponent<RigidBodyComponent>(info.OtherId);

                glm::vec3 displacement = -info.CollisionNormal * info.Overlap;
                if (rb2) displacement /= 2.0f;
                transform->Translation += displacement;

                // Compute impulse on current object 

                // Coefficient of restitution
                // e = (v2_f - v1_f) / (v2_i - v1_i)
                // e = 1 -> perfectly elastic
                // e = 0 -> perfectly inelastic
                // 0 < e < 1 -> inelastic
                float e = 1.0f;

                float m1 = rb->Mass;
                glm::vec3 v1_i = rb->Velocity;

                float m2 = rb2 ? rb2->Mass : std::numeric_limits<float>::max();
                glm::vec3 v2_i = rb2 ? rb2->Velocity : glm::vec3(0.0f);

                float impulseMag = ((m1 * m2) / (m1 + m2)) * (1.0f + e) * glm::dot(v2_i - v1_i, -info.CollisionNormal);
                glm::vec3 direction = glm::normalize(transform->Translation - info.CollisionPoint);
                glm::vec3 impulse = impulseMag * direction;

                AddImpulse(id, impulse);
            }

            glm::vec3 angularVelocity = glm::mat3(transform->Rotation)
                * glm::inverse(DiagonalMat(rb->InverseInitialMomentOfInertia))
                * glm::transpose(glm::mat3(transform->Rotation))
                * rb->AngularMomentum;

            // Calculate rotation matrix derivative
            glm::mat4 rDerivative{
                glm::vec4(glm::cross(angularVelocity, glm::vec3(transform->Rotation[0])), 0.0f),
                glm::vec4(glm::cross(angularVelocity, glm::vec3(transform->Rotation[1])), 0.0f),
                glm::vec4(glm::cross(angularVelocity, glm::vec3(transform->Rotation[2])), 0.0f),
                glm::vec4(0)
            };

            transform->Rotation += rDerivative * dt;

            rb->Velocity += rb->Acceleration * dt;
            transform->Translation += rb->Velocity * dt;

            LogTransform(id, transform);
            LogRigidbody(id, rb);
            if (m_Ecs->HasComponent<ColliderComponent>(id)) {
                LogCollider(id, m_Ecs->GetComponent<ColliderComponent>(id));
            }

            rb->Acceleration = glm::vec3(0);
        }

        m_Logger.Record("Step", m_StepCount);
        m_Logger.Record("Time", m_StepCount * (double)STEP_SIZE);

        m_StepCount++;
    }

    void PhysicsSystem::LogTransform(Id id, const TransformComponent* transform) {
        LogPath path = ObjectLogPath(id) / "Transform";
        m_Logger.Record(path / "Translation", transform->Translation);
        m_Logger.Record(path / "Rotation (Euler Angles)", transform->EulerAngles());
        m_Logger.Record(path / "Scale", transform->Scale);
    }

    void PhysicsSystem::LogRigidbody(Id id, const RigidBodyComponent* rb) {
        LogPath path = ObjectLogPath(id) / "Rigidbody";
        m_Logger.Record(path / "Velocity", rb->Velocity);
        m_Logger.Record(path / "Acceleration", rb->Acceleration);
        m_Logger.Record(path / "AngularMomentum", rb->AngularMomentum);
        m_Logger.Record(path / "Mass", rb->Mass);
        m_Logger.Record(path / "EnableGravity", rb->EnableGravity);
        m_Logger.Record(path / "Speed", rb->Speed());
    }

    void PhysicsSystem::LogCollider(Id id, const ColliderComponent* col) {
        LogPath path = ObjectLogPath(id) / "Collider";
        if (auto* box = std::get_if<ColliderComponent::Box>(&col->Shape)) {
            m_Logger.Record(path / "Shape", "Box");
            m_Logger.Record(path / "Box Dimensions", box->Dimensions);
        }
        else if (auto* sphere = std::get_if<ColliderComponent::Sphere>(&col->Shape)) {
            m_Logger.Record(path / "Shape", "Sphere");
            m_Logger.Record(path / "Sphere Radius", sphere->Radius);
        }
    }
}

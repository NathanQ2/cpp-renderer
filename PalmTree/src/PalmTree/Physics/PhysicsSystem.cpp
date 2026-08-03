// Implementation based off derivation from blackedout01 on YouTube https://youtu.be/4r_EvmPKOvY

#include "PhysicsSystem.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "implot.h"
#include "PalmTree/EntityComponentSystem/EntityComponentSystem.h"

namespace PalmTree {
    void PhysicsSystem::OnRegistered() {
        m_CollisionSystem = m_Ecs->GetSystem<CollisionSystem>();
        PT_CORE_ASSERT(m_CollisionSystem, "CollisionSystem must be registered before PhysiscsSystem");
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
        RigidBodyComponent& rb = m_Ecs->GetComponent<RigidBodyComponent>(id);
        rb.Acceleration += force / rb.Mass;
        
        glm::vec3 torque = glm::cross(offset, force);
        rb.AngularMomentum += torque * STEP_SIZE;
        
        #ifdef PT_DEBUG
        m_DebugInfo[id].Forces.emplace_back(force, offset);
        #endif
    }

    void PhysicsSystem::AddForce(GameObject& obj, glm::vec3 force, glm::vec3 offset) {
        AddForce(obj.GetId(), force, offset);
    }
    
    void PhysicsSystem::AddImpulse(Id id, glm::vec3 impulse) {
        RigidBodyComponent& rb = m_Ecs->GetComponent<RigidBodyComponent>(id);
        rb.Velocity += impulse / rb.Mass;
        
        #ifdef PT_DEBUG
        m_DebugInfo[id].Impulses.emplace_back(impulse);
        #endif
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
            TransformComponent& t = m_Ecs->GetComponent<TransformComponent>(id);
            RigidBodyComponent& rb = m_Ecs->GetComponent<RigidBodyComponent>(id);
            totalKineticEnergy += 0.5f * rb.Mass * glm::length(rb.Velocity);
            
            if (rb.EnableGravity) totalPotentialEnergy += rb.Mass * t.Translation.y * -1;
        }
        float totalEnergy = totalKineticEnergy + totalPotentialEnergy;
        ImGui::Text("Total Kinetic Energy: %f", totalKineticEnergy);
        ImGui::Text("Total Potential Energy: %f", totalPotentialEnergy);
        ImGui::Text("Total Energy: %f", totalEnergy);
        
        ImGui::Separator();
        
        #ifdef PT_DEBUG
        
        ImGui::Text("Object Info");
        int i = 0;
        for (auto& kv : m_DebugInfo) {
            ImGui::PushID(i);
            std::string treeName = fmt::format("ID: {}", kv.first);
            if (ImGui::TreeNodeEx(treeName.c_str())) {
                std::shared_ptr<TransformComponent> transform = kv.second.Transform;
                if (transform && ImGui::TreeNodeEx("Transform")) {
                    ImGui::Text("Transform.Translation: (%f, %f, %f)", transform->Translation.x, transform->Translation.y, transform->Translation.z);
                    ImGui::Text("Transform.Rotation (Euler Angles): (%f, %f, %f)", transform->EulerAngles().x, transform->EulerAngles().y, transform->EulerAngles().z);
                    ImGui::Text("Transform.Scale: (%f, %f, %f)", transform->Scale.x, transform->Scale.y, transform->Scale.z);
                    
                    ImGui::TreePop();
                }
                else if (!transform) {
                    ImGui::Text("No transform");
                }
            
                std::shared_ptr<RigidBodyComponent> rb = kv.second.RigidBody;
                if (rb && ImGui::TreeNodeEx("RigidBody")) {
                    ImGui::Text("RigidBody.Velocity: (%f, %f, %f)", rb->Velocity.x, rb->Velocity.y, rb->Velocity.z);
                    ImGui::Text("RigidBody.Acceleration: (%f, %f, %f)", rb->Acceleration.x, rb->Acceleration.y, rb->Acceleration.z);
                    ImGui::Text("RigidBody.AngularMomentum: (%f, %f, %f)", rb->AngularMomentum.x, rb->AngularMomentum.y, rb->AngularMomentum.z);
                    
                    ImGui::TreePop();
                }
                else if (!rb) {
                    ImGui::Text("No rigidbody");
                }
            
                std::shared_ptr<ColliderComponent> col = kv.second.Collider;
                if (col && ImGui::TreeNodeEx("Collider")) {
                    if (ColliderComponent::Box* box = std::get_if<ColliderComponent::Box>(&col->Shape)) {
                        ImGui::Text("Collider (Box)");
                        ImGui::Text("Collider.Shape.Dimensions: (%f, %f, %f)", box->Dimensions.x, box->Dimensions.y, box->Dimensions.z);
                    }
                    else if (ColliderComponent::Sphere* sphere = std::get_if<ColliderComponent::Sphere>(&col->Shape)) {
                        ImGui::Text("Collider (Sphere)");
                        ImGui::Text("Collider.Shape.Radius: %f", sphere->Radius);
                    }
                    
                    ImGui::TreePop();
                }
                else if (!col) {
                    ImGui::Text("No collider");
                }
            
                if (ImGui::TreeNodeEx("Forces")) {
                    for (int i2 = 0; i2 < kv.second.Forces.size(); i2++) {
                        ImGui::Text(
                            "%i: Force: (%f, %f, %f), Offset: (%f, %f, %f)", 
                            i2,
                            kv.second.Forces[i2].first.x,
                            kv.second.Forces[i2].first.y,
                            kv.second.Forces[i2].first.z,
                            kv.second.Forces[i2].second.x,
                            kv.second.Forces[i2].second.y,
                            kv.second.Forces[i2].second.z
                        );
                    }
                    ImGui::TreePop();
                }
            
                if (ImGui::TreeNodeEx("Impulses")) {
                    for (int i2 = 0; i2 < kv.second.Impulses.size(); i2++) {
                        ImGui::Text(
                            "%i: Impulse: (%f, %f, %f)", 
                            i2,
                            kv.second.Impulses[i2].x,
                            kv.second.Impulses[i2].y,
                            kv.second.Impulses[i2].z
                        );
                    }
                    ImGui::TreePop();
                }
                
                ImGui::TreePop();
            }
            
            ImGui::PopID();
            i++;
        }
        
        #endif
        ImGui::End();
    }

    void PhysicsSystem::Step(float dt) {
        if (dt == 0.0f) return;
        
        m_CollisionSystem->Update();
        
        for (Id id : m_Ids) {
            #ifdef PT_DEBUG
            m_DebugInfo[id].Forces.clear();
            m_DebugInfo[id].Impulses.clear();
            #endif
            TransformComponent& transform = m_Ecs->GetComponent<TransformComponent>(id);
            RigidBodyComponent& rb = m_Ecs->GetComponent<RigidBodyComponent>(id);
            
            if (rb.EnableGravity) {
                AddForce(id, glm::vec3(0.0f, 9.8067 * rb.Mass, 0.0f));
            }
            
            std::vector<CollisionSystem::CollisionInfo>& infos = m_CollisionSystem->GetCollisionInfos(id);
            for (auto& info : infos) {
                TransformComponent& t2 = m_Ecs->GetComponent<TransformComponent>(info.OtherId);
                bool rb2Exists = m_Ecs->HasComponent<RigidBodyComponent>(info.OtherId);
                RigidBodyComponent& rb2 = m_Ecs->GetComponent<RigidBodyComponent>(info.OtherId);
                
                glm::vec3 displacement = -info.CollisionNormal * info.Overlap;
                if (rb2Exists) displacement /= 2.0f;
                transform.Translation += displacement;
                
                // Compute impulse on current object 
                
                // Coefficient of restitution
                // e = (v2_f - v1_f) / (v2_i - v1_i)
                // e = 1 -> perfectly elastic
                // e = 0 -> perfectly inelastic
                // 0 < e < 1 -> inelastic
                float e = 1.0f;
                
                float m1 = rb.Mass;
                glm::vec3 v1_i = rb.Velocity; 
                
                float m2 = rb2Exists ? rb2.Mass : std::numeric_limits<float>::max();
                glm::vec3 v2_i = rb2Exists ? rb2.Velocity : glm::vec3(0.0f);
                
                float impulseMag = ((m1 * m2) / (m1 + m2)) * (1.0f + e) * glm::dot(v2_i - v1_i, -info.CollisionNormal);
                glm::vec3 direction = glm::normalize(transform.Translation - info.CollisionPoint);
                glm::vec3 impulse = impulseMag * direction;
                
                // PT_CORE_TRACE("Impulse (Id = {}):\n    m1 = {}, v1_i = {}\n    m2 = {}, v2_i = {}\n    impulseMag = {}\n    direction = {}\n    impulse = {}", id, m1, glm::to_string(v1_i), m2, glm::to_string(v2_i), impulseMag, glm::to_string(direction), glm::to_string(impulse));
                AddImpulse(id, impulse);
            }

            glm::vec3 angularVelocity = glm::mat3(transform.Rotation)
                * glm::inverse(DiagonalMat(rb.InverseInitialMomentOfInertia))
                * glm::transpose(glm::mat3(transform.Rotation))
                * rb.AngularMomentum;

            // Calculate rotation matrix derivative
            glm::mat4 rDerivative{
                glm::vec4(glm::cross(angularVelocity, glm::vec3(transform.Rotation[0])), 0.0f),
                glm::vec4(glm::cross(angularVelocity, glm::vec3(transform.Rotation[1])), 0.0f),
                glm::vec4(glm::cross(angularVelocity, glm::vec3(transform.Rotation[2])), 0.0f),
                glm::vec4(0)
            };

            transform.Rotation += rDerivative * dt;

            rb.Velocity += rb.Acceleration * dt;
            transform.Translation += rb.Velocity * dt;
            
            #ifdef PT_DEBUG
            m_DebugInfo[id].Id = id;
            m_DebugInfo[id].Step = m_StepCount;
            m_DebugInfo[id].Time = m_StepCount * (double)STEP_SIZE;
            m_DebugInfo[id].Transform = std::make_shared<TransformComponent>(transform);
            m_DebugInfo[id].RigidBody = std::make_shared<RigidBodyComponent>(rb);
            if (m_Ecs->HasComponent<ColliderComponent>(id)) {
                m_DebugInfo[id].Collider = std::make_shared<ColliderComponent>(m_Ecs->GetComponent<ColliderComponent>(id));
            }
            else {
                m_DebugInfo[id].Collider = nullptr;
            }
            #endif
            
            LogTransform(id, transform);
            LogRigidbody(id, rb);
            if (m_Ecs->HasComponent<ColliderComponent>(id)) {
                LogCollider(id, m_Ecs->GetComponent<ColliderComponent>(id));
            }

            rb.Acceleration = glm::vec3(0);
        }
        
        m_Logger.Record("Step", m_StepCount);
        m_Logger.Record("Time", m_StepCount * (double)STEP_SIZE);
        
        m_StepCount++;
    }

    void PhysicsSystem::LogTransform(Id id, const TransformComponent& transform) {
        LogPath path = ObjectLogPath(id) / "Transform";
        m_Logger.Record(path / "Translation", transform.Translation);
        m_Logger.Record(path / "Rotation (Euler Angles)", transform.EulerAngles());
        m_Logger.Record(path / "Scale", transform.Scale);
    }

    void PhysicsSystem::LogRigidbody(Id id, const RigidBodyComponent& rb) {
        LogPath path = ObjectLogPath(id) / "Rigidbody";
        m_Logger.Record(path / "Velocity", rb.Velocity);
        m_Logger.Record(path / "Acceleration", rb.Acceleration);
        m_Logger.Record(path / "AngularMomentum", rb.AngularMomentum);
        m_Logger.Record(path / "Mass", rb.Mass);
        m_Logger.Record(path / "EnableGravity", rb.EnableGravity);
        m_Logger.Record(path / "Speed", rb.Speed());
    }

    void PhysicsSystem::LogCollider(Id id, const ColliderComponent& col) {
        LogPath path = ObjectLogPath(id) / "Collider";
        if (auto* box = std::get_if<ColliderComponent::Box>(&col.Shape)) {
            m_Logger.Record(path / "Shape", "Box");
            m_Logger.Record(path / "Box Dimensions", box->Dimensions);
        }
        else if (auto* sphere = std::get_if<ColliderComponent::Sphere>(&col.Shape)) {
            m_Logger.Record(path / "Shape", "Sphere");
            m_Logger.Record(path / "Sphere Radius", sphere->Radius);
        }
    }
}

#include "CollisionSystem.h"

#include <glm/ext/matrix_projection.hpp>

namespace PalmTree {
    CollisionSystem::CollisionSystem() {
        for (auto& vec : m_Collisions) {
            vec.reserve(5);
        }
    }
    
    void CollisionSystem::Update() {
        for (std::vector<CollisionInfo>& collisions : m_Collisions) {
            collisions.clear();
        }
        
        for (Id id1 : m_Ids) {
            for (Id id2 : m_Ids) {
                if (id1 == id2) continue;
                
                TransformComponent* t1 = m_Ecs->GetComponent<TransformComponent>(id1);
                ColliderComponent* c1 = m_Ecs->GetComponent<ColliderComponent>(id1);
                
                TransformComponent* t2 = m_Ecs->GetComponent<TransformComponent>(id2);
                ColliderComponent* c2 = m_Ecs->GetComponent<ColliderComponent>(id2);
                
                std::visit([id1, &t1, &c1, id2, &t2, &c2, this](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    
                    if constexpr (std::is_same_v<T, ColliderComponent::Sphere>)
                        SphereCollision(id1, t1, c1, id2, t2, c2);
                    else if constexpr (std::is_same_v<T, ColliderComponent::Box>)
                        BoxCollision(id1, t1, c1, id2, t2, c2);
                    else PT_CORE_ASSERT(false, "Unsupported collision");
                }, c1->Shape);
            }
        }
    }

    void CollisionSystem::SphereCollision(Id id1, TransformComponent* t1, ColliderComponent* c1, Id id2, TransformComponent* t2, ColliderComponent* c2) {
        std::visit([id1, t1, c1, id2, t2, c2, this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
                    
            if constexpr (std::is_same_v<T, ColliderComponent::Sphere>)
                SphereSphereCollision(id1, t1, c1, id2, t2, c2);
            else if constexpr (std::is_same_v<T, ColliderComponent::Box>)
                BoxSphereCollision(id2, t2, c2, id1, t1, c1);
            else PT_CORE_ASSERT(false, "Unsupported collision");
        }, c2->Shape);
    }

    void CollisionSystem::BoxCollision(
        Id id1,
        TransformComponent* t1,
        ColliderComponent* c1,
        Id id2,
        TransformComponent* t2,
        ColliderComponent* c2
    ) {
        std::visit([id1, t1, c1, id2, t2, c2, this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
                    
            if constexpr (std::is_same_v<T, ColliderComponent::Sphere>)
                BoxSphereCollision(id1, t1, c1, id2, t2, c2);
            else {}
            // else PT_CORE_ASSERT(false, "Unsupported collision");
        }, c2->Shape);
    }

    void CollisionSystem::SphereSphereCollision(
        Id id1,
        TransformComponent* t1,
        ColliderComponent* c1,
        Id id2,
        TransformComponent* t2,
        ColliderComponent* c2
    ) {
        ColliderComponent::Sphere* s1 = std::get_if<ColliderComponent::Sphere>(&c1->Shape);
        ColliderComponent::Sphere* s2 = std::get_if<ColliderComponent::Sphere>(&c2->Shape);
        
        PT_CORE_ASSERT(s1, "Collider 1 must be a sphere!");
        PT_CORE_ASSERT(s2, "Collider 2 must be a sphere!");
        
        float dist = glm::distance(t1->Translation, t2->Translation);
                
        if (dist < s1->Radius + s2->Radius) {
            glm::vec3 directionTo2 = glm::normalize(t2->Translation - t1->Translation); 
            glm::vec3 collisionPoint = directionTo2 * dist / 2.0f + t1->Translation;
                    
            float overlap = s1->Radius + s2->Radius - dist;
                    
            m_Collisions[id1].emplace_back(collisionPoint, directionTo2, overlap, id2);
            m_Collisions[id2].emplace_back(collisionPoint, -directionTo2, overlap, id1);
        }
    }

    void CollisionSystem::BoxSphereCollision(
        Id id1,
        TransformComponent* t1,
        ColliderComponent* c1,
        Id id2,
        TransformComponent* t2,
        ColliderComponent* c2
    ) {
        // Sphere is colliding with box if, for any point (P) on sphere,
        // Box.width / 2 < P.x < Box.width / 2 and
        // Box.length / 2 < P.y < Box.length / 2 and
        // Box.height / 2 < P.z < Box.height / 2 and
        // If Box is centered at origin and is axis aligned
        
        ColliderComponent::Box* b1 = std::get_if<ColliderComponent::Box>(&c1->Shape);
        ColliderComponent::Sphere* s2 = std::get_if<ColliderComponent::Sphere>(&c2->Shape);
        
        PT_CORE_ASSERT(b1, "Collider 1 must be a box");
        PT_CORE_ASSERT(s2, "Collider 2 must be a sphere");
        
        glm::vec3 sTranslationInBox = glm::inverse(glm::mat3(t1->Rotation)) * (t2->Translation - t1->Translation);
        
        float halfWidth = b1->Dimensions.x / 2.0f;
        float halfHeight = b1->Dimensions.y / 2.0f;
        float halfLength = b1->Dimensions.z / 2.0f;
        
        // Check vertex collisions
        const std::array vertices {
            glm::vec3(halfWidth, halfHeight, halfLength),
            glm::vec3(halfWidth, halfHeight, -halfLength),
            glm::vec3(halfWidth, -halfHeight, halfLength),
            glm::vec3(halfWidth, -halfHeight, -halfLength),
            glm::vec3(-halfWidth, halfHeight, halfLength),
            glm::vec3(-halfWidth, halfHeight, -halfLength),
            glm::vec3(-halfWidth, -halfHeight, halfLength),
            glm::vec3(-halfWidth, -halfHeight, -halfLength)
        };
        
        for (glm::vec3 vertex : vertices) {
            float dist = glm::length(sTranslationInBox - vertex);
            if (dist < s2->Radius) {
                glm::vec3 collisionNormal = glm::normalize(sTranslationInBox - vertex); 
                glm::vec3 collisionPoint = vertex;
                    
                float overlap = s2->Radius - dist;
                    
                m_Collisions[id1].emplace_back(collisionPoint, collisionNormal, overlap, id2);
                m_Collisions[id2].emplace_back(collisionPoint, -collisionNormal, overlap, id1);
                
                return;
            }
        }
        
        // Check collision with box faces
        std::array sExtremities{
            sTranslationInBox + glm::vec3(s2->Radius, 0.0f, 0.0f),
            sTranslationInBox - glm::vec3(s2->Radius, 0.0f, 0.0f),
            
            sTranslationInBox + glm::vec3(0.0f, s2->Radius, 0.0f),
            sTranslationInBox - glm::vec3(0.0f, s2->Radius, 0.0f),
            
            sTranslationInBox + glm::vec3(0.0f, 0.0f, s2->Radius),
            sTranslationInBox - glm::vec3(0.0f, 0.0f, s2->Radius),
        };
        
        for (glm::vec3 extremity : sExtremities) {
            if (IsBetween(extremity.x, -halfWidth, halfWidth) &&
                IsBetween(extremity.y, -halfHeight, halfHeight) &&
                IsBetween(extremity.z, -halfLength, halfLength)) {
                glm::vec3 collisionPoint = glm::mat3(t1->Rotation) * extremity;
                glm::vec3 collisionNormal = glm::normalize(glm::mat3(t1->Rotation) * (sTranslationInBox - extremity));
                
                glm::vec3 facePosition = glm::vec3(halfWidth, halfHeight, halfLength) * glm::normalize(sTranslationInBox - extremity);
                float overlap = 0.0f;
                if (facePosition.x == 0.0f && facePosition.z == 0.0f) overlap = halfHeight - extremity.y;
                if (facePosition.x == 0.0f && facePosition.y == 0.0f) overlap = halfLength - extremity.z;
                if (facePosition.y == 0.0f && facePosition.z == 0.0f) overlap = halfWidth - extremity.x;
                
                m_Collisions[id1].emplace_back(collisionPoint, collisionNormal, overlap, id2);
                m_Collisions[id2].emplace_back(collisionPoint, -collisionNormal, overlap, id1);
            }
        }
    }

    bool CollisionSystem::IsBetween(float x, float min, float max) {
        return x < max && x > min;
    }
}

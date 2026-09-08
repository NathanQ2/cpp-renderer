#pragma once

#include "PalmTree/EntityComponentSystem/EntityComponentSystem.h"

namespace PalmTree {
    class CollisionSystem : public System {
    public:
        struct CollisionInfo {
            // Point in world space that collision occurred
            glm::vec3 CollisionPoint{0.0f};
            // Vector that points away from the colliding face of this object
            glm::vec3 CollisionNormal{0.0f};
            // The length that the two colliding bodies overlap along the CollisionNormal vector 
            float Overlap = 0.0f;
            // The id of the other colliding object
            Id OtherId;
        };

        CollisionSystem();

        void Update();

        std::vector<CollisionInfo>& GetCollisionInfos(Id id) {
            return m_Collisions[id];
        }
    private:
        void SphereCollision(
            Id id1,
            TransformComponent* t1,
            ColliderComponent* c1,
            Id id2,
            TransformComponent* t2,
            ColliderComponent* c2
        );
        void BoxCollision(
            Id id1,
            TransformComponent* t1,
            ColliderComponent* c1,
            Id id2,
            TransformComponent* t2,
            ColliderComponent* c2
        );

        void SphereSphereCollision(
            Id id1,
            TransformComponent* t1,
            ColliderComponent* c1,
            Id id2,
            TransformComponent* t2,
            ColliderComponent* c2
        );
        void BoxSphereCollision(
            Id id1,
            TransformComponent* t1,
            ColliderComponent* c1,
            Id id2,
            TransformComponent* t2,
            ColliderComponent* c2
        );

        static bool IsBetween(float x, float min, float max);

        std::array<std::vector<CollisionInfo>, MAX_GAME_OBJECTS> m_Collisions;
    };
}

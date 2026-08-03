#pragma once

#include "../Model.h"

#include <variant>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "PalmTree/Utils.h"

namespace PalmTree {
    struct TransformComponent {
        glm::vec3 Translation{};
        glm::vec3 Scale{1.0f};
        glm::mat4 Rotation = glm::identity<glm::mat4>();

        void SetEuler(glm::vec3 euler);
        glm::mat4 TransformationMatrix() const;
        glm::vec3 EulerAngles() const;
        glm::mat3 NormalMatrix() const;
    };

    struct PointLightComponent {
        float LightIntensity = 1.0f;
        glm::vec3 Color{};
    };

    struct ModelComponent {
        glm::vec3 Color{};
        std::shared_ptr<Model> Model{};
    };

    struct RigidBodyComponent {
        glm::vec3 Velocity = glm::vec3(0);
        glm::vec3 Acceleration = glm::vec3(0);

        // Assume 1x1x1 cube for now
        glm::vec3 InverseInitialMomentOfInertia = GetDiagonal(
            glm::inverse((1.0f / 12.0f) * 1 * glm::mat3{{2, 0, 0}, {0, 2, 0}, {0, 0, 2}})
        );

        glm::vec3 AngularMomentum = glm::vec3(0);

        float Mass = 1.0f;
        bool EnableGravity = false;
        
        float Speed() const { return glm::length(Velocity); }
    };
    
    struct ColliderComponent {
        struct Box {
            glm::vec3 Dimensions{1.0f};
        };
        
        struct Sphere {
            float Radius = 1.0f;
        };
        
        std::variant<Box, Sphere> Shape;
    };
}
